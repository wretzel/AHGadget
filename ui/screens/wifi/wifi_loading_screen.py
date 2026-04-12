from kivy.uix.screenmanager import Screen
from kivy.clock import Clock

from system.wifi.layer3.api import start_engine, get_live_networks
from ui.widgets.loading.loading_widget import LoadingWidget


class WifiLoadingScreen(Screen):
    def __init__(self, **kwargs):
        super().__init__(**kwargs)

        # ---------------------------------------------------------
        # UNIVERSAL LOADING WIDGET
        # ---------------------------------------------------------
        self.loading = LoadingWidget()
        self.add_widget(self.loading)

        # internal state
        self.elapsed = 0
        self.timeout = 12

    # ---------------------------------------------------------
    # LAYOUT
    # ---------------------------------------------------------
    def on_size(self, *args):
        self.loading.size = self.size
        self.loading.pos = self.pos

    def on_pos(self, *args):
        self.on_size()

    # ---------------------------------------------------------
    # ENTER / EXIT
    # ---------------------------------------------------------
    def on_enter(self):
        self.elapsed = 0
        start_engine()

        self._poll = Clock.schedule_interval(self.check_ready, 0.5)

    def on_leave(self):
        if hasattr(self, "_poll"):
            self._poll.cancel()

    # ---------------------------------------------------------
    # ENGINE READINESS
    # ---------------------------------------------------------
    def check_ready(self, dt):
        nets = get_live_networks()
        if nets or self.elapsed >= self.timeout:
            self.goto_overview()
        self.elapsed += dt

    def goto_overview(self):
        if hasattr(self, "_poll"):
            self._poll.cancel()

        self.manager.current = "wifi_overview"
