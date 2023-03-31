import matplotlib.pyplot as plt

def plot_losses(x1, name):
        """The method plots the calculated loss values for training and validation
        """
        plt.plot(x1)
        plt.title(name)
        plt.show()

instructions_gpp = [
    12426224058,
    12806544442,
    12451757627,
    12071488924,
    12448157725,
    12451782020,
    5158741244
]

plot_losses(instructions_gpp, "Instructions")