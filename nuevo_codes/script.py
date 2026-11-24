
from pathlib import Path
import pandas as pd
import re
import matplotlib.pyplot as plt

def guardar_figura(nombre):
    directorio = Path("RESULTADOS")
    directorio.mkdir(exist_ok=True)

    plt.savefig(
        directorio / nombre,
        dpi=300,
        bbox_inches="tight"
    )

def cargar_resultados(directorio):
    patrones = {
        "f_eval(S) (inicial)": r"f_eval\(S\) \(inicial\)\s*=\s*(\d+)",
        "f_eval(S)": r"f_eval\(S\)\s*=\s*(\d+)",
        "Duracion en segundos": r"Duracion en segundos:\s*([\d.]+)",
        "MAX_RESTARTS": r"MAX_RESTARTS:\s*(\d+)",
        "MAX_ITERACIONES_POR_HC": r"MAX_ITERACIONES_POR_HC:\s*(\d+)"
    }

    registros = []

    for archivo in Path(directorio).glob("*.ctt"):
        contenido = archivo.read_text(
            encoding="utf-8", errors="replace"
        )

        registro = {
            "Nombre archivo": archivo.name,
            "Instancia": re.sub(
                r"^\d+_iters_", "", archivo.stem
            )
        }

        for columna, patron in patrones.items():
            resultado = re.search(patron, contenido)
            registro[columna] = (
                resultado.group(1) if resultado else None
            )

            if resultado is None:
                print(f"Advertencia: falta {columna} en {archivo.name}")

        registros.append(registro)

    columnas = [
        "f_eval(S) (inicial)",
        "f_eval(S)",
        "Duracion en segundos",
        "MAX_RESTARTS",
        "MAX_ITERACIONES_POR_HC",
        "Instancia",
        "Nombre archivo"
    ]

    df = pd.DataFrame(registros, columns=columnas)

    columnas_numericas = [
    "f_eval(S) (inicial)",
    "f_eval(S)",
    "Duracion en segundos",
    "MAX_RESTARTS",
    "MAX_ITERACIONES_POR_HC"
]

    df[columnas_numericas] = df[columnas_numericas].apply(
        pd.to_numeric, errors="coerce"
    )

    return df


def preparar_datos(df, incluir_toy=False):
    datos = df.copy()

    if not incluir_toy:
        datos = datos[datos["Instancia"].str.startswith("comp")]

    datos["MAX_ITERACIONES_POR_HC"] = pd.to_numeric(
        datos["MAX_ITERACIONES_POR_HC"]
    )

    return datos.sort_values(
        ["Instancia", "MAX_ITERACIONES_POR_HC"]
    )


def analizar_estadisticas(df):
    datos = preparar_datos(df)

    resumen = datos.groupby("MAX_ITERACIONES_POR_HC").agg(
        f_eval_promedio=("f_eval(S)", "mean"),
        f_eval_mediana=("f_eval(S)", "median"),
        f_eval_minimo=("f_eval(S)", "min"),
        f_eval_maximo=("f_eval(S)", "max"),
        tiempo_promedio=("Duracion en segundos", "mean"),
        tiempo_mediano=("Duracion en segundos", "median"),
        tiempo_minimo=("Duracion en segundos", "min"),
        tiempo_maximo=("Duracion en segundos", "max")
    )

    return resumen.round(2)

def grafico_calidad(df):
    datos = preparar_datos(df)

    grupos = [
        datos.loc[
            datos["MAX_ITERACIONES_POR_HC"] == n,
            "f_eval(S)"
        ]
        for n in [10, 100, 1000]
    ]

    plt.figure(figsize=(8, 5))
    plt.boxplot(grupos, tick_labels=["10", "100", "1000"])

    plt.title("Distribución de la calidad de las soluciones")
    plt.xlabel("Máximo de iteraciones por Hill Climbing")
    plt.ylabel("f_eval(S) - Menor es mejor")
    plt.grid(axis="y", alpha=0.3)
    plt.tight_layout()
    guardar_figura("calidad.png")
    plt.show()


def grafico_tiempo(df):
    datos = preparar_datos(df)

    tiempos = datos.groupby(
        "MAX_ITERACIONES_POR_HC"
    )["Duracion en segundos"].mean()

    plt.figure(figsize=(8, 5))
    plt.bar(tiempos.index.astype(str), tiempos.values)

    plt.title("Tiempo promedio de ejecución")
    plt.xlabel("Máximo de iteraciones por Hill Climbing")
    plt.ylabel("Tiempo (segundos)")
    plt.grid(axis="y", alpha=0.3)
    plt.tight_layout()
    guardar_figura("tiempo_promedio.png")
    plt.show()


def grafico_tiempo_calidad(df):
    datos = preparar_datos(df)

    plt.figure(figsize=(8, 5))

    for iteraciones, grupo in datos.groupby(
        "MAX_ITERACIONES_POR_HC"
    ):
        plt.scatter(
            grupo["Duracion en segundos"],
            grupo["f_eval(S)"],
            label=f"{iteraciones} iteraciones"
        )

    plt.title("Relación entre tiempo y calidad")
    plt.xlabel("Tiempo de ejecución (segundos)")
    plt.ylabel("f_eval(S) - Menor es mejor")
    plt.legend()
    plt.grid(alpha=0.3)
    plt.tight_layout()
    guardar_figura("tiempo_vs_calidad.png")
    plt.show()


def grafico_mejoras_instancia(df):
    datos = preparar_datos(df)

    tabla = datos.pivot(
        index="Instancia",
        columns="MAX_ITERACIONES_POR_HC",
        values="f_eval(S)"
    )

    mejoras = pd.DataFrame(index=tabla.index)

    mejoras["10 a 100"] = (
        (tabla[10] - tabla[100]) / tabla[10].replace(0, float("nan"))
    ) * 100

    mejoras["100 a 1000"] = (
        (tabla[100] - tabla[1000]) / tabla[100].replace(0, float("nan"))
    ) * 100

    mejoras.plot(
        kind="bar",
        figsize=(12, 6),
        width=0.8
    )

    plt.title("Mejora porcentual por instancia")
    plt.xlabel("Instancia")
    plt.ylabel("Reducción de f_eval (%)")
    plt.axhline(0, linewidth=0.8)
    plt.xticks(rotation=45)
    plt.legend(title="Configuración")
    plt.grid(axis="y", alpha=0.3)
    plt.tight_layout()
    guardar_figura("mejoras_por_instancia.png")
    plt.show()



if __name__ == '__main__':
    df = cargar_resultados("./OUTPUTS")
    
    #print(df.sort_values(['Instancia','MAX_ITERACIONES_POR_HC']))
    df.to_csv("RESULTADOS/dataset_resultados_experimento.csv", index=False)
    # Estadísticas
    est = analizar_estadisticas(df)
    print(est)
    est.to_csv(
        "RESULTADOS/estadisticas.csv",
        index = True
        )

    # Gráficos
    grafico_calidad(df)
    grafico_tiempo(df)
    grafico_tiempo_calidad(df)
    grafico_mejoras_instancia(df)



'''
# Ejemplo de uso
df = cargar_resultados("./OUTPUTS")

df.to_csv("resultados_experimento.csv", index=False)

'''
