# wifi_overview_decor.py

from kivy.uix.image import Image
from kivy.uix.label import Label


def make_background():
    return Image(
        source="assets/backgrounds/wifi/wifi_overview_bg.png",
        allow_stretch=True,
        keep_ratio=False
    )


def make_header():
    return Label(
        text="Wi‑Fi Overview",
        font_size="24sp",
        size_hint=(1, None),
        height=40
    )


def make_section_title(text):
    return Label(
        text=text,
        font_size="20sp",
        size_hint=(1, None),
        height=30
    )
