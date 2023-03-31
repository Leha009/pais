import matplotlib.pyplot as plt

def plot_losses(cpp: list, clang: list, xticks: list, name:str):
        for item in [cpp, clang, xticks]:
            item.insert(1, item.pop(3))

        plt.plot(cpp, label="g++")
        plt.plot(clang, label="Clang++")
        plt.title(name)
        plt.legend()
        plt.xticks([i for i in range(len(xticks))], xticks, rotation=45)
        plt.show()

XTICKS = [
    "Просто цикл",
    "OpenMP",
    "Матрица как 1d",
    "Без функции",
    "Без функции + OpenMP",
    "OpenMP векторизация",
    "Векторизация mavx2"
]

instructions_gpp = [
    12426224058,
    12806544442,
    12451757627,
    12071488924,
    12448157725,
    12451782020,
    5158741244
]

instructions_clang = [
    7477459452,
    9085993769,
    9896290372,
    7122339459,
    8405152802,
    11256350614,
    5376077094
]


plot_losses(instructions_gpp, instructions_clang, XTICKS, "Instructions")

Time_gpp = [
    0.664025,
    0.120439,
    0.120736,
    0.649109,
    0.11789,
    0.109875,
    0.060152
]

Time_clang = [
    0.637477,
    0.120782,
    0.125856,
    0.637468,
    0.115037,
    0.123346,
    0.082163
]

XTICKS = [
    "Просто цикл",
    "OpenMP",
    "Матрица как 1d",
    "Без функции",
    "Без функции + OpenMP",
    "OpenMP векторизация",
    "Векторизация mavx2"
]


plot_losses(Time_gpp, Time_clang, XTICKS, "Time")