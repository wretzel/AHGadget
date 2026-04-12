#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <math.h>

#include <xf86drm.h>
#include <xf86drmMode.h>
#include <png.h>

#define FADE_STEPS 40
#define FADE_DELAY_US 8000   // ~60 FPS

struct drm_fb {
    int fd;
    uint32_t fb_id;
    uint32_t crtc_id;
    uint32_t connector_id;
    uint32_t width;
    uint32_t height;
    uint32_t pitch;
    uint32_t handle;
    uint8_t *map;
    size_t size;
};

static void load_png(const char *filename, uint8_t **out, int *w, int *h) {
    FILE *fp = fopen(filename, "rb");
    if (!fp) { perror("fopen"); exit(1); }

    png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    png_infop info = png_create_info_struct(png);
    png_init_io(png, fp);
    png_read_info(png, info);

    *w = png_get_image_width(png, info);
    *h = png_get_image_height(png, info);

    png_set_expand(png);
    png_set_strip_16(png);
    png_set_add_alpha(png, 0xFF, PNG_FILLER_AFTER);
    png_set_gray_to_rgb(png);

    png_bytep *rows = malloc(sizeof(png_bytep) * (*h));
    uint8_t *img = malloc((*w) * (*h) * 4);

    for (int i = 0; i < *h; i++)
        rows[i] = img + i * (*w) * 4;

    png_read_image(png, rows);

    free(rows);
    fclose(fp);
    png_destroy_read_struct(&png, &info, NULL);

    *out = img;
}

static void blend_and_draw(struct drm_fb *fb, uint8_t *img, int alpha) {
    for (int y = 0; y < fb->height; y++) {
        uint32_t *row = (uint32_t *)(fb->map + y * fb->pitch);
        uint8_t *src = img + y * fb->width * 4;

        for (int x = 0; x < fb->width; x++) {
            uint8_t r = src[x*4 + 0];
            uint8_t g = src[x*4 + 1];
            uint8_t b = src[x*4 + 2];

            r = (r * alpha) / 255;
            g = (g * alpha) / 255;
            b = (b * alpha) / 255;

            row[x] = (r << 16) | (g << 8) | b;
        }
    }
}

int main(int argc, char **argv) {
    if (argc < 2) {
        printf("Usage: %s splash.png\n", argv[0]);
        return 1;
    }

    // Load PNG
    uint8_t *img;
    int w, h;
    load_png(argv[1], &img, &w, &h);

    struct drm_fb fb = {0};
    fb.fd = open("/dev/dri/card0", O_RDWR);
    if (fb.fd < 0) { perror("open drm"); exit(1); }

    drmModeRes *res = drmModeGetResources(fb.fd);
    if (!res) { fprintf(stderr, "drmModeGetResources failed\n"); exit(1); }

    drmModeConnector *conn = NULL;
    drmModeEncoder *enc = NULL;

    // Find connected connector
    for (int i = 0; i < res->count_connectors; i++) {
        drmModeConnector *tmp = drmModeGetConnector(fb.fd, res->connectors[i]);
        if (tmp->connection == DRM_MODE_CONNECTED && tmp->count_modes > 0) {
            conn = tmp;
            fb.connector_id = conn->connector_id;
            break;
        }
        drmModeFreeConnector(tmp);
    }

    if (!conn) {
        fprintf(stderr, "No connected connector found\n");
        exit(1);
    }

    // Get encoder
    enc = drmModeGetEncoder(fb.fd, conn->encoder_id);
    if (!enc) {
        fprintf(stderr, "Failed to get encoder\n");
        exit(1);
    }

    fb.crtc_id = enc->crtc_id;

    // Use preferred mode
    drmModeModeInfo mode = conn->modes[0];
    fb.width = mode.hdisplay;
    fb.height = mode.vdisplay;

    // Create dumb buffer
    struct drm_mode_create_dumb creq = {0};
    creq.width = fb.width;
    creq.height = fb.height;
    creq.bpp = 32;
    ioctl(fb.fd, DRM_IOCTL_MODE_CREATE_DUMB, &creq);

    fb.pitch = creq.pitch;
    fb.size = creq.size;
    fb.handle = creq.handle;

    drmModeAddFB(fb.fd, fb.width, fb.height, 24, 32, fb.pitch, fb.handle, &fb.fb_id);
    drmModeSetCrtc(fb.fd, fb.crtc_id, fb.fb_id, 0, 0, &fb.connector_id, 1, &mode);

    struct drm_mode_map_dumb mreq = {0};
    mreq.handle = fb.handle;
    ioctl(fb.fd, DRM_IOCTL_MODE_MAP_DUMB, &mreq);

    fb.map = mmap(0, fb.size, PROT_READ | PROT_WRITE, MAP_SHARED, fb.fd, mreq.offset);

    // Fade in
    for (int a = 0; a <= 255; a += (255 / FADE_STEPS)) {
        blend_and_draw(&fb, img, a);
        usleep(FADE_DELAY_US);
    }

    // Hold
    blend_and_draw(&fb, img, 255);
    usleep(3000000);

    // Cleanup
    munmap(fb.map, fb.size);
    drmModeRmFB(fb.fd, fb.fb_id);

    struct drm_mode_destroy_dumb dreq = {0};
    dreq.handle = fb.handle;
    ioctl(fb.fd, DRM_IOCTL_MODE_DESTROY_DUMB, &dreq);

    drmModeFreeEncoder(enc);
    drmModeFreeConnector(conn);
    drmModeFreeResources(res);

    close(fb.fd);
    free(img);

    return 0;
}
