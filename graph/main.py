import matplotlib.pyplot as plt
import numpy as np
import pandas as pd
from matplotlib.backends.backend_pdf import PdfPages


def read(fn):
    df = pd.read_csv('data.csv', index_col=0)
    return df


def main():
    data = read("data.csv")
    pdf = PdfPages('g.pdf')
    sets = [
        ["clang-mac", "g++-mac"],
        ["clang-rp32", "g++-rp32"],
        ["clang-rp64", "g++-rp64"],
        ["cl32-win", "cl64-win"]
    ]
    fig = plt.figure()
    for gix in range(len(sets)):
        s = sets[gix]
        ax = fig.add_subplot(2, 2, gix+1)
        ax.set_title(", ".join(s))
        names = list(reversed(list(data.transpose().columns)))
        x_pos = np.arange(len(names))*1.0
        ax.set_yticks(x_pos + 0.2)
        ax.set_yticklabels(names)
        for e in s:
            ticks = list(reversed(list(data[e].values)))
            ax.barh(x_pos, ticks, height=0.4, label=e)
            x_pos += 0.4
        ax.legend(fontsize=8)
    plt.tight_layout()
    pdf.savefig()
    pdf.close()


main()
