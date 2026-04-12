from ui.widgets.tiles.neon_tile import NeonTile


def setup_buttons(screen):
    layout = screen.layout

    tiles = [
        ("IR",        [0.9, 0.2, 0.2, 1],  screen.go_ir),
        ("RF",        [0.2, 0.6, 1.0, 1],  screen.go_rf),
        ("WiFi",      [0.3, 1.0, 0.4, 1],  screen.go_wifi),
        ("BLE",       [1.0, 0.8, 0.2, 1],  screen.go_ble),
        ("USB",       [0.8, 0.3, 1.0, 1],  screen.go_usb),
        ("System",    [0.4, 0.4, 0.4, 1],  screen.go_system),
    ]

    positions = [
        {"center_x": 0.1, "center_y": 0.08},
        {"center_x": 0.3, "center_y": 0.08},
        {"center_x": 0.5, "center_y": 0.08},
        {"center_x": 0.7, "center_y": 0.08},
        {"center_x": 0.9, "center_y": 0.08},
        {"center_x": 3,   "center_y": 0.08},
    ]

    for (label, color, callback), pos in zip(tiles, positions):
        tile = NeonTile(
            text=label,
            fill_color=color,
            size_hint=(0.20, 0.15),
            pos_hint=pos
        )
        tile.bind(on_release=callback)
        layout.add_widget(tile)

    back_tile = NeonTile(
        text="Back",
        fill_color=[0.4, 0.4, 0.4, 1],
        size_hint=(0.20, 0.15),
        pos_hint={"center_x": 0.5, "center_y": 0.25}
    )
    back_tile.bind(on_release=screen.go_start)
    layout.add_widget(back_tile)
