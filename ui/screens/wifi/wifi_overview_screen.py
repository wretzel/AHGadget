# wifi_overview_screen.py

from kivy.uix.screenmanager import Screen
from kivy.uix.boxlayout import BoxLayout
from kivy.clock import Clock

from system.wifi.layer3.api import (
    get_wifi_weather,
    get_best_networks,
    get_health_report,
    stop_engine
)

from .wifi_overview_decor import (
    make_background,
    make_header,
    make_section_title
)

from .wifi_overview_sections import (
    BestNetworksSection,
    WeatherSummarySection,
    StatsSection
)


class WifiOverviewScreen(Screen):
    def __init__(self, **kwargs):
        super().__init__(**kwargs)

        # background
        self.bg = make_background()
        self.add_widget(self.bg)

        # main vertical stack
        self.content = BoxLayout(
            orientation="vertical",
            spacing=10,
            padding=10,
            size_hint=(1, 0.88),
            pos_hint={"top": 1}
        )
        self.add_widget(self.content)

        # header
        self.content.add_widget(make_header())

        # best networks
        self.content.add_widget(make_section_title("Best Networks"))
        self.best_section = BestNetworksSection()
        self.content.add_widget(self.best_section)

        # full list button
        from kivy.uix.button import Button
        self.full_list_btn = Button(
            text="Full networks list →",
            size_hint=(1, None),
            height=40
        )
        self.full_list_btn.bind(on_release=self.go_list)
        self.content.add_widget(self.full_list_btn)

        # weather summary
        self.summary_section = WeatherSummarySection(
            text="Loading Wi‑Fi Weather...",
            font_size="18sp",
            size_hint=(1, None),
            height=50
        )
        self.content.add_widget(self.summary_section)

        # stats
        self.content.add_widget(make_section_title("Stats to Know"))
        self.stats_section = StatsSection()
        self.content.add_widget(self.stats_section)

        # bottom nav
        from kivy.uix.button import Button
        nav = BoxLayout(size_hint=(1, 0.12), spacing=10, padding=10)

        btn_home = Button(text="Home")
        btn_home.bind(on_release=self.go_home)

        btn_next = Button(text="View Networks")
        btn_next.bind(on_release=self.go_list)

        nav.add_widget(btn_home)
        nav.add_widget(btn_next)
        self.add_widget(nav)

    def on_enter(self):
        self.refresh()
        self._timer = Clock.schedule_interval(lambda dt: self.refresh(), 5)

    def on_leave(self):
        if hasattr(self, "_timer"):
            self._timer.cancel()

    def refresh(self):
        self.summary_section.update(get_wifi_weather())
        self.best_section.update(get_best_networks(limit=3))
        self.stats_section.update(get_health_report().stats)

    def go_home(self, *args):
        stop_engine()
        self.manager.current = "home"

    def go_list(self, *args):
        self.manager.current = "wifi_list"
