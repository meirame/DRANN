#!/usr/bin/env python3
"""Representación del historial; no interviene en el entrenamiento de DRANN."""

import argparse
import csv
import math
from pathlib import Path
import sys


def load_history(path):
    epochs, training, validation = [], [], []
    with path.open(newline="", encoding="utf-8") as history:
        reader = csv.DictReader(history)
        required = {"epoch", "training_loss", "validation_loss"}
        if not required.issubset(reader.fieldnames or []):
            raise ValueError("El CSV debe contener epoch, training_loss y validation_loss.")
        for line, row in enumerate(reader, start=2):
            try:
                epoch = int(row["epoch"])
                train_loss = float(row["training_loss"])
                validation_loss = float(row["validation_loss"])
            except (ValueError, TypeError) as error:
                raise ValueError(f"Valores no válidos en la línea {line}.") from error
            if epoch < 0 or (epochs and epoch <= epochs[-1]):
                raise ValueError(f"Las épocas deben ser crecientes: línea {line}.")
            if not all(math.isfinite(value) and value >= 0
                       for value in (train_loss, validation_loss)):
                raise ValueError(f"Pérdida negativa o no finita en la línea {line}.")
            epochs.append(epoch)
            training.append(train_loss)
            validation.append(validation_loss)
    if not epochs:
        raise ValueError("El historial no contiene evaluaciones.")
    return epochs, training, validation


def plot_history(epochs, training, validation, log_y=False):
    import matplotlib.pyplot as plt

    if log_y and any(value <= 0 for value in training + validation):
        raise ValueError("La escala logarítmica requiere pérdidas mayores que cero.")
    figure, axis = plt.subplots(figsize=(10, 6), layout="constrained")
    axis.plot(epochs, training, label="Entrenamiento", color="#2563eb", linewidth=2)
    axis.plot(epochs, validation, label="Validación", color="#ea580c", linewidth=2)
    axis.set_title("DRANN — evolución de la pérdida", fontsize=15, pad=14)
    axis.set_xlabel("Época e")
    axis.set_ylabel("Pérdida geométrica normalizada")
    if log_y:
        axis.set_yscale("log")
    else:
        axis.set_ylim(bottom=0)
    axis.grid(True, which="major", alpha=0.25)
    axis.legend(frameon=False)
    axis.spines["top"].set_visible(False)
    axis.spines["right"].set_visible(False)
    return figure


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("history", nargs="?", type=Path,
                        default=Path("training_history.csv"))
    parser.add_argument("--output", type=Path, help="Ruta de la imagen PNG.")
    parser.add_argument("--no-show", action="store_true",
                        help="Guardar la gráfica sin abrir una ventana.")
    parser.add_argument("--log-y", action="store_true",
                        help="Usar escala logarítmica para observar pérdidas pequeñas.")
    args = parser.parse_args()
    try:
        import matplotlib.pyplot as plt
    except ImportError:
        print("Falta Matplotlib. Instala requirements-plot.txt en tu entorno Python.",
              file=sys.stderr)
        return 1

    try:
        epochs, training, validation = load_history(args.history)
        figure = plot_history(epochs, training, validation, args.log_y)
        output = args.output or args.history.with_suffix(".png")
        figure.savefig(output, dpi=160)
        print(f"Gráfica guardada: {output.resolve()}")
        if not args.no_show:
            if plt.get_backend().lower() == "agg":
                print("Este entorno no tiene un backend gráfico interactivo. "
                      "Puedes abrir el PNG; para la ventana necesitas, por ejemplo, TkAgg.")
            else:
                plt.show()
        plt.close(figure)
    except (OSError, ValueError) as error:
        print(f"No se pudo representar el historial: {error}", file=sys.stderr)
        return 1
    return 0


if __name__ == "__main__":
    sys.exit(main())
