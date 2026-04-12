import platform
from kivy.core.window import Window
from kivy.config import Config

def is_pi():
    return platform.system() == "Linux" and platform.machine().startswith("arm")

def configure_environment():
    if is_pi():
        # Real hardware behavior
        Window.fullscreen = True
        Window.show_cursor = False

        # Touchscreen input provider
        Config.set('input', 'touchscreen', 'mtdev,/dev/input/event0')
    else:
        Window.fullscreen = False
        Window.size = (1280, 800)
        Window.resizable = False
        Window.show_cursor = True

