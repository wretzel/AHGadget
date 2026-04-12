from kivy.uix.screenmanager import Screen
from kivy.uix.image import Image
from kivy.uix.button import Button
from kivy.uix.boxlayout import BoxLayout
from kivy.uix.label import Label
from kivy.clock import Clock

from system.wifi.layer3.api import get_health_report


class WifiDetailsScreen(Screen):
    def __init__(self, **kwargs):
        super().__init__(**kwargs)

        # Background
        self.bg = Image(
            source="assets/backgrounds/wifi/wifi_details_bg.png",
            allow_stretch=True,
            keep_ratio=False
        )
        self.add_widget(self.bg)

        # MAIN CONTENT AREA
        self.content = BoxLayout(
            orientation="vertical",
            spacing=10,
            padding=10,
            size_hint=(1, 0.88),
            pos_hint={"top": 1}
        )
        self.add_widget(self.content)

        # Header
        self.header = Label(
            text="Network Details",
            font_size="24sp",
            size_hint=(1, None),
            height=40
        )
        self.content.add_widget(self.header)

        # Info labels (filled in by set_network)
        self.info_labels = {}

        for key in ["SSID", "BSSID", "Signal", "Channel", "Band", "Hidden"]:
            lbl = Label(
                text=f"{key}: ",
                font_size="18sp",
                size_hint=(1, None),
                height=30
            )
            self.info_labels[key] = lbl
            self.content.add_widget(lbl)

        # Diagnostics placeholder
        self.diagnostics_label = Label(
            text="Diagnostics loading...",
            font_size="16sp",
            size_hint=(1, None),
            height=60
        )
        self.content.add_widget(self.diagnostics_label)

        # Bottom navigation bar
        nav = BoxLayout(
            size_hint=(1, 0.12),
            pos_hint={"x": 0, "y": 0},
            spacing=10,
            padding=10
        )

        btn_back = Button(text="Back to List")
        btn_back.bind(on_release=self.go_list)

        nav.add_widget(btn_back)
        self.add_widget(nav)

        # Internal storage
        self.network = None

    # Called by List screen
    def set_network(self, network):
        self.network = network
        self.update_info()

    # Update UI with network details
    def update_info(self):
        if not self.network:
            return

        n = self.network

        band = "2.4 GHz" if 2400 <= (n.freq_mhz or 0) <= 2500 else \
               "5 GHz" if 5000 <= (n.freq_mhz or 0) <= 6000 else "Unknown"

        self.info_labels["SSID"].text = f"SSID: {n.ssid or '(hidden)'}"
        self.info_labels["BSSID"].text = f"BSSID: {n.bssid}"
        self.info_labels["Signal"].text = f"Signal: {n.signal_dbm} dBm"
        self.info_labels["Channel"].text = f"Channel: {n.channel}"
        self.info_labels["Band"].text = f"Band: {band}"
        self.info_labels["Hidden"].text = f"Hidden: {n.hidden}"

        # Pull deeper analysis
        report = get_health_report()
        diags = report.diagnostics

        # Normalize diagnostics into a flat list of strings
        flat = []

        if isinstance(diags, dict):
            diags = list(diags.values())

        if not isinstance(diags, list):
            diags = [diags]

        for item in diags:
            if isinstance(item, list):
                # flatten nested lists
                for sub in item:
                    flat.append(str(sub))
            else:
                flat.append(str(item))

        # Now safe to slice and join
        diag_text = "\n".join(flat[:3]) if flat else "No diagnostics"
        self.diagnostics_label.text = f"Diagnostics:\n{diag_text}"



    # Navigation
    def go_list(self, *args):
        self.manager.current = "wifi_list"
