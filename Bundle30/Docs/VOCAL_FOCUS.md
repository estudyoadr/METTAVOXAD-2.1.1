# MettaVoxAD Vocal FOCUS — 3.0.3

Novo motor optativo, com quatro presets adicionados após os seis anteriores.

- Signature U87: ponto de partida para locução, não emulação física de microfone.
- Impact Clean: maior controle dinâmico para chamadas fortes.
- Velvet Close: menor compressão e brilho para locução próxima.
- Radio Presence: presença mais evidente, com peso moderado.

Novas instâncias abrem Signature U87, FOCUS ativo e Entrada -6 dB.
Os presets anteriores chamam o motor clássico. Estados sem parâmetro FOCUS
restauram o motor clássico; índices anteriores de parâmetros e programas ficam
preservados. FOCUS e MATCH foram adicionados ao final da lista de parâmetros.

FOCUS: detector do leveller filtrado em 120 Hz, joelho 9 dB, peak catcher
independente até 4 dB, redução dinâmica de médios graves, corpo em 110 Hz,
presença 2.8–3.2 kHz, brilho em 10 kHz, proteção de sibilância até 4.5 dB,
saturação com resíduos não lineares por bandas e coeficientes de EQ atualizados
por valores suavizados a cada 1 ms. Não é limiter nem correção espectral universal.

MATCH: compensação aproximada por RMS, suavizada, limitada a +/-9 dB e congelada
na ausência de sinal útil. Serve para comparação com BYPASS; não é LUFS match
nem garantia de igualdade perceptiva. Deixe desligado na exportação final se
quiser decidir o volume manualmente. Os WAVs de demonstração foram igualados
por LUFS offline e usam MATCH desligado.

Primeiro trate a voz. Robótico, pitch e efeitos especiais vêm depois.
Não aplique esta cadeia corretiva automaticamente em produções já finalizadas.
A aprovação de timbre depende da escuta das comparações pelo usuário.
