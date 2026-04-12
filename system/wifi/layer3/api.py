# Mock Wi-Fi API for Windows UI development

import time
from dataclasses import dataclass

# --- Data models ---

@dataclass
class LiveNetwork:
    ssid: str
    bssid: str
    signal_dbm: int
    freq_mhz: int
    channel: int
    hidden: bool

@dataclass
class StatsItem:
    label: str
    value: str
    icon: str

@dataclass
class HealthReport:
    overall_score: int
    overall_icon: str
    summary: str
    ticker: list
    stats: list
    metrics: dict
    scores: dict
    diagnostics: list


# --- Internal mock engine state ---

_engine_start_time = None
_engine_delay = 8  # seconds before networks appear (simulate Pi behavior)


# --- Engine lifecycle (mocked) ---

def start_engine():
    global _engine_start_time
    _engine_start_time = time.time()
    print("[MOCK] start_engine() called")


def stop_engine():
    print("[MOCK] stop_engine() called")


# --- Live networks (List screen) ---

def get_live_networks():
    # Simulate engine warm-up delay
    if _engine_start_time is None:
        return []

    elapsed = time.time() - _engine_start_time
    if elapsed < _engine_delay:
        return []  # engine not ready yet

    # After delay, return mock networks
    return [
        LiveNetwork("HomeWiFi", "AA:BB:CC:DD:EE:01", -45, 2412, 1, False),
        LiveNetwork("CoffeeShop", "AA:BB:CC:DD:EE:02", -70, 5180, 36, False),
        LiveNetwork("", "AA:BB:CC:DD:EE:03", -82, 2462, 11, True),
    ]


# --- Best networks (Overview screen) ---

def get_best_networks(limit=3):
    nets = get_live_networks()
    return nets[:limit]


# --- Weather summary (Overview screen) ---

def get_wifi_weather():
    return {
        "overall": {
            "score": 72,
            "icon": "📶",
            "summary": "Moderate congestion, stable environment"
        },
        "ticker": [
            "2.4GHz crowded",
            "5GHz clear",
            "Strong AP nearby"
        ],
        "stats": [
            {"label": "Density", "value": "High", "icon": "🌐"},
            {"label": "Congestion", "value": "Medium", "icon": "📡"},
            {"label": "Interference", "value": "Low", "icon": "⚡"},
        ]
    }


# --- Full analysis (Details screen) ---

def get_health_report():
    return HealthReport(
        overall_score=72,
        overall_icon="📶",
        summary="Moderate congestion, stable environment",
        ticker=["2.4GHz crowded", "5GHz clear", "Strong AP nearby"],
        stats=[
            StatsItem("Density", "High", "🌐"),
            StatsItem("Congestion", "Medium", "📡"),
            StatsItem("Interference", "Low", "⚡"),
        ],
        metrics={"density": 0.8, "congestion": 0.5},
        scores={"quality": 72},
        diagnostics=[
            "2.4GHz band is crowded",
            ["Some nested diagnostic list"],
            "5GHz band is clear"
        ]
    )
