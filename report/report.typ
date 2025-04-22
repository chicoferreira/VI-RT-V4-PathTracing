#set page(width: 297mm, height: auto, margin: (x: 5%, y: 5em))
#set text(font: "IBM Plex Sans", lang: "pt", region: "pt")

#set table(
  fill: (x, y) => if (y == 0) {
    gray.lighten(20%)
  } else if (x == 0) {
    gray.lighten(40%)
  },
)

#show link: underline

#let generate(title: [], description: [], paths: dictionary, formula: $$, variables: []) = [
  = #title

  #description

  == Fórmula

  #grid(
    columns: 2,
    column-gutter: 5em,
    box(stroke: black, inset: (x: 1em, y: 0.5em), radius: 5pt, formula), variables,
  )

  == Resultados

  #for entry in paths {
    let rmse = json(entry.rmse_path)
    let spp = entry.spp

    let image_path = rmse.image_path.trim("./report/")
    let image_ref_path = rmse.image_ref_path.trim("./report/")
    let output_path = rmse.output_path.trim("./report/")

    grid(
      columns: 3,
      column-gutter: 1em,
      figure(image(image_path), caption: [#title (#spp SPP)], supplement: none),
      figure(image(image_ref_path), caption: [All Lights (Referência) (#spp SPP)], supplement: none),
      figure(image(output_path), caption: [#title RMSE (#spp SPP)], supplement: none),
    )
  }

  == Tabela de Resultados

  #let entries = for entry in paths {
    let rmse = json(entry.rmse_path)
    let spp = entry.spp
    let hyperfine = json(entry.hyperfine_path).results.at(0)
    let efficiency = 1 / (rmse.rmse * hyperfine.median)
    (
      str(spp),
      [#calc.round(rmse.rmse, digits: 7)],
      [#calc.round(rmse.min_max_scaled_rmse, digits: 7)
        (min: #calc.round(rmse.min_y), max: #calc.round(rmse.max_y, digits: 4))
      ],
      [
        #calc.round(hyperfine.median, digits: 2)s $plus.minus$ #calc.round(hyperfine.stddev, digits: 2) (#hyperfine.times.len() medições)
      ],
      [#calc.round(efficiency, digits: 2)],
    )
  }

  #table(
    columns: 5,
    [SPP], [RMSE], [Min Max Scaled RMSE], [Tempo de Execução (média $plus.minus$ $sigma$)], [Eficiência dos traços],
    ..entries
  )

  #pagebreak(weak: true)
]

#let generate_path(name, spp) = (
  spp: spp,
  rmse_path: "./outputs/rmse" + str(spp) + "_" + name + ".json",
  hyperfine_path: "./outputs/hyperfine_" + str(spp) + "_" + name + ".json",
)

#let generate_paths_spps(name, spps) = spps.map(spp => generate_path(name, spp))

#let spps = (1, 4, 8, 16, 32)

#generate(
  title: [Uniform],
  description: [Selecionar uma luz aleatória uniformemente entre todas as luzes.],
  paths: generate_paths_spps("uniform", spps),
  formula: $
    P_i = 1 / N
  $,
  variables: [- $N$ - número de luzes na cena],
)

#generate(
  title: [Distance],
  description: [A partir de uma CDF gerada a partir da distância desde o ponto atual até à luz.],
  paths: generate_paths_spps("distance", spps),
  formula: $
    P_i = 1 / D
  $,
  variables: [- $D$ - distância entre o ponto atual e a luz],
)

#generate(
  title: [Distance Squared],
  description: [A partir de uma CDF gerada a partir da distância ao quadrado desde o ponto atual até à luz.],
  paths: generate_paths_spps("distance_squared", spps),
  formula: $
    P_i = 1 / D^2
  $,
  variables: [- $D$ - distância entre o ponto atual e a luz],
)

#generate(
  title: [Importance],
  description: [A partir de uma CDF gerada a partir da equação da contribuição de luz para o ponto atual.],
  paths: generate_paths_spps("importance", spps),
  formula: $
    P_i = (I times cos(alpha) times cos(beta) times A) / D^2
  $,
  variables: [
    - $I$ - Luminância da intensidade da luz
    - $arrow(L)$ - Vetor *normalizado* desde o ponto de interseção até à luz
    - $cos(alpha)$ - Ângulo do vetor $arrow(L)$ com a normal da superfície atual ($= arrow(L) dot arrow(N)$, já que $||arrow(L)|| = ||arrow(N)|| = 1$)
    - $cos(beta)$ - Ângulo do vetor $-arrow(L)$ com a normal da geometria da luz ($= -arrow(L) dot arrow(N_L)$, já que $||arrow(L)|| = ||arrow(N_L)|| = 1$) ($=1$ se a luz for um ponto)
    - $A$ - Área da luz (se a luz for um ponto, $A=1$)
    - $D$ - Distância entre o ponto atual e a luz
  ],
)

#generate(
  title: [Importance No Distance],
  description: [],
  paths: generate_paths_spps("importance_no_distance", spps),
  formula: $
    P_i = I times cos(alpha) times cos(beta) times A
  $,
  variables: [
    - $I$ - Luminância da intensidade da luz
    - $arrow(L)$ - Vetor *normalizado* desde o ponto de interseção até à luz
    - $cos(alpha)$ - Ângulo do vetor $arrow(L)$ com a normal da superfície atual ($= arrow(L) dot arrow(N)$, já que $||arrow(L)|| = ||arrow(N)|| = 1$)
    - $cos(beta)$ - Ângulo do vetor $-arrow(L)$ com a normal da geometria da luz ($= -arrow(L) dot arrow(N_L)$, já que $||arrow(L)|| = ||arrow(N_L)|| = 1$) ($=1$ se a luz for um ponto)
    - $A$ - Área da luz (se a luz for um ponto, $A=1$)
  ],
)

= Imagens Resultados Agregadas

#let paths = (
  (name: "uniform", title: "Uniform"),
  (name: "distance", title: "Distance"),
  (name: "distance_squared", title: "Distance Squared"),
  (name: "importance_no_distance", title: "Importance No Distance"),
  (name: "importance", title: "Importance"),
  (name: "all_lights", title: "All Lights"),
)

#let generate_aggregated_images(paths, spp) = [
  #let paths = paths.map(entry => (
    entry + (image_path: "./outputs/output" + str(spp) + "_" + entry.name + ".png")
  ))

  == #spp SPP

  #grid(
    columns: 3,
    row-gutter: 1em,
    column-gutter: 1em, ..paths.map(entry => figure(
      image(entry.image_path),
      caption: [#entry.title (#spp SPP)],
      supplement: none,
    ))
  )
]

#let generate_aggregated_table(paths, spp) = [
  #let paths = paths.map(entry => (
    entry + generate_path(entry.name, spp)
  ))

  #table(
    columns: 5,
    [Sampler (#spp SPP)], [RMSE], [Min Max Scaled RMSE], [Tempo de Execução (média $plus.minus$ $sigma$)], [Eficiência dos traços],
    ..paths
      .map(entry => {
        let hyperfine = json(entry.hyperfine_path).results.at(0)

        let exec_content = [
          #calc.round(hyperfine.median, digits: 2)s $plus.minus$ #calc.round(hyperfine.stddev, digits: 2) (#hyperfine.times.len() medições)
        ]

        if (entry.name == "all_lights") {
          return (entry.title, [--], [--], exec_content, [--])
        }

        let rmse = json(entry.rmse_path)
        let efficiency = 1 / (rmse.rmse * hyperfine.median)
        (
          entry.title,
          [#calc.round(rmse.rmse, digits: 7)],
          [#calc.round(rmse.min_max_scaled_rmse, digits: 7)
            (min: #calc.round(rmse.min_y), max: #calc.round(rmse.max_y, digits: 4))
          ],
          exec_content,
          [#calc.round(efficiency, digits: 2)],
        )
      })
      .flatten()
  )
]

#for spp in spps {
  generate_aggregated_images(paths, spp)
}

= Tabela de Resultados Agregados

#for spp in spps {
  generate_aggregated_table(paths, spp)
}


#let spp = 1

#let paths = (
  (name: "uniform", title: "Uniform"),
  (name: "distance", title: "Distance"),
  (name: "distance_squared", title: "Distance Squared"),
  (name: "importance_no_distance", title: "Importance No Distance"),
  (name: "importance", title: "Importance"),
  (name: "all_lights", title: "All Lights"),
).map(entry => (
  entry + (image_path: "./outputs/output" + str(spp) + "_" + entry.name + ".png") + generate_path(entry.name, spp)
))

#pagebreak()

= Metodologia

- A cena utilizada para os testes é a cena `DLightChallenge`.
- O tempo de execução foi medido utilizando o `hyperfine`, exportando o resultado para JSON.
- Os dados e imagem resultante do RMSE foi gerada a partir do #link("https://github.com/chicoferreira/raytracer-rmse")[raytracer-rmse].
- Os dados foram gerados a partir do script `generate.sh`.
- Os testes foram realizados em um computador com as seguintes especificações:
  - CPU: AMD Ryzen 7 7700X
  - RAM: DDR5 6000 MHz $times$ 2
