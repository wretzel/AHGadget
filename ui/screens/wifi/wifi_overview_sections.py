# wifi_overview_sections.py

from kivy.uix.boxlayout import BoxLayout
from kivy.uix.label import Label


class BestNetworksSection(BoxLayout):
    def __init__(self, **kwargs):
        super().__init__(orientation="vertical", spacing=5,
                         size_hint=(1, None), height=120, **kwargs)

    def update(self, nets):
        self.clear_widgets()

        if not nets:
            self.add_widget(Label(text="No networks found"))
            return

        for n in nets:
            self.add_widget(Label(
                text=f"{n.ssid}  ({n.signal_dbm} dBm, Ch {n.channel})",
                font_size="16sp",
                size_hint=(1, None),
                height=30
            ))


class WeatherSummarySection(Label):
    def update(self, weather):
        summary = weather["overall"]["summary"]
        score = weather["overall"]["score"]
        icon = weather["overall"]["icon"]
        self.text = f"{icon}  {summary}  ({score}/100)"


class StatsSection(BoxLayout):
    def __init__(self, **kwargs):
        super().__init__(orientation="vertical", spacing=5,
                         size_hint=(1, None), height=120, **kwargs)

    def update(self, stats):
        self.clear_widgets()

        if not stats:
            self.add_widget(Label(text="No stats available"))
            return

        for item in stats[:3]:
            self.add_widget(Label(
                text=f"{item.icon}  {item.label}: {item.value}",
                font_size="16sp",
                size_hint=(1, None),
                height=30
            ))
