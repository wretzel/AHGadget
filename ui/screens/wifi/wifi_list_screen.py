from kivy.uix.screenmanager import Screen
from kivy.uix.image import Image
from kivy.uix.button import Button
from kivy.uix.boxlayout import BoxLayout
from kivy.uix.label import Label
from kivy.uix.scrollview import ScrollView
from kivy.clock import Clock

from system.wifi.layer3.api import get_live_networks


class WifiListScreen(Screen):
    def __init__(self, **kwargs):
        super().__init__(**kwargs)

        # Background
        self.bg = Image(
            source="assets/backgrounds/wifi/wifi_list_bg.png",
            allow_stretch=True,
            keep_ratio=False
        )
        self.add_widget(self.bg)

        # MAIN CONTENT AREA (above bottom nav)
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
            text="Available Networks",
            font_size="24sp",
            size_hint=(1, None),
            height=40
        )
        self.content.add_widget(self.header)

        # Scrollable list container
        self.scroll = ScrollView(size_hint=(1, 1))
        self.content.add_widget(self.scroll)

        self.list_container = BoxLayout(
            orientation="vertical",
            spacing=5,
            size_hint_y=None
        )
        self.list_container.bind(minimum_height=self.list_container.setter("height"))
        self.scroll.add_widget(self.list_container)

        # Bottom navigation bar
        nav = BoxLayout(
            size_hint=(1, 0.12),
            pos_hint={"x": 0, "y": 0},
            spacing=10,
            padding=10
        )

        btn_back = Button(text="Overview")
        btn_back.bind(on_release=self.go_overview)

        nav.add_widget(btn_back)

        self.add_widget(nav)

    # Called when entering the screen
    def on_enter(self):
        self.update_list()
        self._event = Clock.schedule_interval(self.update_list, 5)

    # Called when leaving the screen
    def on_leave(self):
        if hasattr(self, "_event"):
            self._event.cancel()

    # Update the list of networks
    def update_list(self, *args):
        networks = get_live_networks()

        self.list_container.clear_widgets()

        if not networks:
            self.list_container.add_widget(Label(text="No networks found"))
            return

        for net in networks:
            row = Button(
                text=f"{net.ssid or '(hidden)'}  |  {net.signal_dbm} dBm  |  Ch {net.channel}",
                size_hint_y=None,
                height=40
            )
            row.bind(on_release=lambda x, n=net: self.open_details(n))
            self.list_container.add_widget(row)

    # Navigation methods
    def go_overview(self, *args):
        self.manager.current = "wifi_overview"

    def open_details(self, network):
        # Pass the network object to the details screen
        details = self.manager.get_screen("wifi_details")
        details.set_network(network)
        self.manager.current = "wifi_details"
