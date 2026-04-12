from kivy.app import App
from kivy.uix.screenmanager import ScreenManager, FadeTransition
from kivy.uix.floatlayout import FloatLayout

import sys, os

# Add project root to Python path
PROJECT_ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), ".."))
sys.path.insert(0, PROJECT_ROOT)

from ui.core.env import configure_environment
from ui.screens.start.start_screen import StartScreen
from ui.screens.home.home_screen import HomeScreen

# NEW: Wi-Fi screens
from ui.screens.wifi.wifi_overview_screen import WifiOverviewScreen
from ui.screens.wifi.wifi_list_screen import WifiListScreen
from ui.screens.wifi.wifi_details_screen import WifiDetailsScreen
from ui.screens.wifi.wifi_loading_screen import WifiLoadingScreen

from ui.widgets.cursor_effects import CursorEffects


class AHGadgetApp(App):
    def build(self):
        configure_environment()

        # Root layout that holds everything
        root = FloatLayout()

        # Screen manager (middle layer)
        sm = ScreenManager(transition=FadeTransition(duration=0.15))

        # Core screens
        sm.add_widget(StartScreen(name="start"))
        sm.add_widget(HomeScreen(name="home"))

        # Wi-Fi subsystem screens
        sm.add_widget(WifiOverviewScreen(name="wifi_overview"))
        sm.add_widget(WifiListScreen(name="wifi_list"))
        sm.add_widget(WifiDetailsScreen(name="wifi_details"))
        sm.add_widget(WifiLoadingScreen(name="wifi_loading"))

        sm.current = "start"

        # Global cursor layer (top layer)
        cursor = CursorEffects()

        # Add layers in correct order
        root.add_widget(sm)       # bottom
        root.add_widget(cursor)   # top

        return root


if __name__ == "__main__":
    AHGadgetApp().run()
