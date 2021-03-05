# language_override:  "en"
from engine import TR1ItemId, I18n

from scripts.tr1.object_infos import object_infos
from scripts.tr1.audio import tracks
from scripts.tr1.level_sequence import level_sequence, title_menu, lara_home
from hid import GlfwKey, GlfwGamepadButton, Action

if False:
    cheats = {
        "godMode": True,
        "allAmmoCheat": True,
        "inventory": {
            TR1ItemId.Key1: 10,
            TR1ItemId.Key2: 10,
            TR1ItemId.Key3: 10,
            TR1ItemId.Key4: 10,
            TR1ItemId.Puzzle1: 10,
            TR1ItemId.Puzzle2: 10,
            TR1ItemId.Puzzle3: 10,
            TR1ItemId.Puzzle4: 10,
        }
    }
else:
    cheats = {}

input_mapping = {
    Action.MoveSlow: [GlfwKey.LeftShift, GlfwGamepadButton.RightBumper],
    Action.Action: [GlfwKey.LeftControl, GlfwGamepadButton.A],
    Action.Holster: [GlfwKey.R, GlfwGamepadButton.Y],
    Action.Jump: [GlfwKey.Space, GlfwGamepadButton.X],
    Action.Roll: [GlfwKey.X, GlfwGamepadButton.B],
    Action.FreeLook: [GlfwKey.Kp0, GlfwGamepadButton.LeftBumper],
    Action.Menu: [GlfwKey.Escape, GlfwGamepadButton.Start],
    Action.Debug: [GlfwKey.F11],
    Action.DrawPistols: [GlfwKey.Num1],
    Action.DrawShotgun: [GlfwKey.Num2],
    Action.DrawUzis: [GlfwKey.Num3],
    Action.DrawMagnums: [GlfwKey.Num4],
    Action.ConsumeSmallMedipack: [GlfwKey.Num5],
    Action.ConsumeLargeMedipack: [GlfwKey.Num6],
    Action.Save: [GlfwKey.F5],
    Action.Load: [GlfwKey.F6],
    Action.Left: [GlfwKey.A, GlfwGamepadButton.DPadLeft],
    Action.Right: [GlfwKey.D, GlfwGamepadButton.DPadRight],
    Action.Forward: [GlfwKey.W, GlfwGamepadButton.DPadUp],
    Action.Backward: [GlfwKey.S, GlfwGamepadButton.DPadDown],
    Action.StepLeft: [GlfwKey.Q],
    Action.StepRight: [GlfwKey.E],
    Action.Screenshot: [GlfwKey.F12],
    Action.CheatDive: [GlfwKey.F10],  # only available in debug builds
}


def getGlidosPack():
    return None
    # return "assets/trx/1SilverlokAllVers/silverlok/"
    # return "assets/trx/JC levels 1-12/Textures/JC/"
    # return "assets/trx/JC levels 13-15/Textures/JC/"


def getObjectInfo(id):
    return object_infos[TR1ItemId(id)]


def getTrackInfo(id):
    return tracks[id]


i18n = {
    I18n.LoadGame: {
        "en": "Load Game",
        "de": "Spiel laden",
    },
    I18n.SaveGame: {
        "en": "Save Game",
        "de": "Spiel sichern",
    },
    I18n.NewGame: {
        "en": "New Game",
        "de": "Spiel starten",
    },
    I18n.ExitGame: {
        "en": "Exit Game",
        "de": "Spiel beenden",
    },
    I18n.ExitToTitle: {
        "en": "Exit to Title",
        "de": "Zur~uck zum Hauptmen~u",
    },
    I18n.EmptySlot: {
        "en": "- EMPTY SLOT %1%",
        "de": "- LEERER SLOT %1%",
    },
    I18n.Items: {
        "en": "ITEMS",
        "de": "Gegen~ande",
    },
    I18n.GameOver: {
        "en": "GAME OVER",
        "de": "Ende des Spiels",
    },
    I18n.Option: {
        "en": "OPTION",
        "de": "Optionen",
    },
    I18n.Inventory: {
        "en": "INVENTORY",
        "de": "Inventar",
    },
    I18n.Game: {
        "en": "Game",
        "de": "Spiel",
    },
    I18n.Controls: {
        "en": "Controls",
        "de": "Kontrollen",
    },
    I18n.Sound: {
        "en": "Sound",
        "de": "Audio",
    },
    I18n.DetailLevels: {
        "en": "Detail Levels",
        "de": "Grafik",
    },
    I18n.LarasHome: {
        "en": "Gym",
        "de": "Training",
    },
    I18n.Compass: {
        "en": "Compass",
        "de": "Kompa=",
    },
    I18n.Pistols: {
        "en": "Pistols",
        "de": "Pistolen",
    },
    I18n.Shotgun: {
        "en": "Shotgun",
        "de": "Schrotflinte",
    },
    I18n.Magnums: {
        "en": "Magnums",
        "de": "Magnums",
    },
    I18n.Uzis: {
        "en": "Uzis",
        "de": "Uzis",
    },
    I18n.Grenade: {
        "en": "Grenade",
        "de": "Granaten",
    },
    I18n.LargeMediPack: {
        "en": "Large Medi Pack",
        "de": "Gro=es Medi-Pack",
    },
    I18n.SmallMediPack: {
        "en": "Small Medi Pack",
        "de": "Kleines Medi-Pack",
    },
    I18n.Loading: {
        "en": "Loading...",
        "de": "Lade...",
    },
    I18n.Saving: {
        "en": "Saving...",
        "de": "Speichere..",
    },
    I18n.BuildingTextures: {
        "en": "Building textures",
        "de": "Erstelle Texturen",
    },
    I18n.LoadingLevel: {
        "en": "Loading %1%",
        "de": "Lade %1%",
    },
    I18n.CreatingMipmaps: {
        "en": "Creating mipmaps (%1%%%)",
        "de": "Erstelle Mipmaps (%1%%%)",
    },
    I18n.LoadingGlidos: {
        "en": "Loading Glidos texture pack",
        "de": "Lade Glidos Textur Paket",
    },
    I18n.GlidosLoading: {
        "en": "Glidos - Loading %1%",
        "de": "Glidos - Lade %1%",
    },
    I18n.GlidosResolvingMaps: {
        "en": "Glidos - Resolving maps (%1%%%)",
        "de": "Glidos - Löse Zuordnungen auf (%1%%%)",
    },
    I18n.ShotgunCells: {
        "en": "Shotgun Cells",
        "de": "Schrot-Munition",
    },
    I18n.MagnumClips: {
        "en": "Magnum Clips",
        "de": "Magnum-Munition",
    },
    I18n.UziClips: {
        "en": "Uzi Clips",
        "de": "Uzi-Munition",
    },
    I18n.CRT: {
        "en": "CRT",
        "de": "CRT",
    },
    I18n.DepthOfField: {
        "en": "Depth-of-Field",
        "de": "Tiefensch~arfe",
    },
    I18n.LensDistortion: {
        "en": "Lens Distortion",
        "de": "Linsenverzerrung",
    },
    I18n.FilmGrain: {
        "en": "Film Grain",
        "de": "Filmrauschen",
    },
    I18n.Fullscreen: {
        "en": "Fullscreen",
        "de": "Vollbild",
    },
    I18n.BilinearFiltering: {
        "en": "Bilinear Filtering",
        "de": "Bilineare Filterung",
    },
    I18n.Graphics: {
        "en": "Graphics",
        "de": "Grafik",
    },
    I18n.WaterDenoise: {
        "en": "Water Denoise",
        "de": "Wassergl~attung",
    },
    I18n.StatsTimeTakenHours: {
        "en": "TIME TAKEN %1%:%2$02d:%3$02d",
        "de": "Ben~otigte Zeit %1%:%2$02d:%3$02d",
    },
    I18n.StatsTimeTaken: {
        "en": "TIME TAKEN %1$02d:%2$02d",
        "de": "Ben~otigte Zeit %1$02d:%2$02d",
    },
    I18n.StatsSecrets: {
        "en": "SECRETS %1% of %2%",
        "de": "Geheimnisse %1% von %2%",
    },
    I18n.StatsPickups: {
        "en": "PICKUPS %1%",
        "de": "Gegenst~ande %1%",
    },
    I18n.StatsKills: {
        "en": "KILLS %1%",
        "de": "Besiegte Gegner %1%",
    },
}

print("Yay! Main script loaded.")
