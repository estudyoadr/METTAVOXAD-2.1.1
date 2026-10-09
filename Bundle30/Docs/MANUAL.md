# MettaVoxAD 3.0 Bundle — primeira versao funcional

Sete plugins independentes, DLL legada e VST3, Windows x86 e x64.
Instale a arquitetura do HOST: Sound Forge 8 usa x86, mesmo em Windows 64 bits.
O instalador do bundle nao substitui nem remove MettaVoxAD 2.1.1.

## Cadeia sugerida
De-Con > The Session Strip > Transient Dynamix > Sonic Matrix > SpaceWeaver > Exciter 808 > Maximum Ceiling.
Nao e preciso usar todos: comece De-Con, Session Strip e Ceiling. Acrescente os demais conforme a locucao.
A captacao, a voz, o instrumental e a escuta determinam o resultado final. Referencia de radio imaging americana e direcao artistica, nao equivalencia comprovada a uma producao ReelWorld.

## Operacao
Cada plugin tem Default e seis presets, entrada/saida em dB, bypass com latencia compensada, parametros automatizaveis e estado salvo pelo host. Sao 42 presets novos no bundle. Os 24 presets originais continuam no 2.1.1.

- **De-Con:** atenuacao espectral WOLA/FFT de 512 pontos, ajuste manual de piso, de-esser de banda, controle de baixas/plosivas e interpolacao de cliques isolados. Ajuste Piso ao ruido real. Reducao excessiva pode criar artefatos. Nao remove toda reverberacao ambiente nem todos estalos de saliva. Gain Staging e opcional. O De-Thump usa filtro/detector de baixas; nao e separacao espectral de plosivas.
- **The Session Strip:** HPF e controle dinamico de graves medios; compressores rapido e optico em serie; saturacao de tres bandas; coesao e sala curta com pre-delay filtrado e ducking. Afinacao monofonica opcional, derivada do 2.1.1, desligada nos presets de locucao. A tecnica granular nao preserva formantes como um editor de afinacao offline e deve ser avaliada em vozes cantadas.
- **Transient Dynamix:** contraste entre envelopes rapido/lento, ataque e sustain, punch de banda, expansao suave e clipping oversampled. Reduza Attack quando a voz ficar dura.
- **Sonic Matrix:** subgrave rastreado na faixa de 40 a 90 Hz, portadora vocoder PolyBLEP seguindo pitch, formantes por remapeamento das bandas, oito estagios all-pass e micro-pitch. XY controla Aura (X), Synth e Sub (Y). Intelligibility reduz wet ate 35% nos eventos de alta frequencia. Sub nao e gerado no silencio. Aura e phaser IIR, nao fase linear. O preset Radio Flyer usa modulacao e formantes; nao executa uma automacao de subida exata de uma oitava.
- **SpaceWeaver:** eco cruzado filtrado e feedback limitado; micro-pitch; ducking. Ritmo 0 = milissegundos; 1 = esquerda 1/8 pontilhado/direita 1/4; 2 = esquerda 1/4 pontilhado/direita 1/4; 3 = esquerda 1/8/direita 1/4. Usa BPM do host quando disponivel, senao 120 BPM.
- **Exciter 808:** saturacao por bandas e shelf Air com protecao de sibilancia. Nao transforma fisicamente um microfone em outro. IPS muda a intensidade da saturacao: 15 ips mais denso, 30 ips mais limpo; nao e emulacao fisica completa de fita.
- **Maximum Ceiling:** caminho oversampled 4x, lookahead 5 ms, detector stereo, reserva de pico para a reconstrucao, medicao independente 8x, LUFS momentaneo/curto/integrado K-weighted com gates absoluto/relativo. O integrado retém ate uma hora e reinicia quando o host prepara o processador. Nao ha certificacao de conformidade nem medicao de codecs. Alvo LUFS orienta o Auto Level opcional e nao garante o loudness integrado final. Dry/Wet muda drive/densidade antes da limitacao; a protecao continua ativa. Ganho positivo de saida e limitado a 0 dB para preservar o teto.

## Perfis de entrega
Radio Wall (-8 LUFS, -0.3 dBTP) e preset criativo agressivo, nao norma universal de radio. Streaming e TV sao pontos de partida: confira a especificacao real de quem recebe o arquivo. Normalizacao das plataformas e controlada por elas.

## Engenharia e limites
Saturacao/clipping/sintese usam oversampling FIR 4x. Crossovers Linkwitz-Riley em 200 Hz e 4 kHz, com alinhamento all-pass da banda grave no somado interno. Nao ha garantias de ausencia absoluta de aliasing ou de resultado artistico; testes e audicao sao necessarios.
Controles foram implementados localmente. Nao ha Pro-Q, Melodyne, 1176, LA-2A, Decapitator, SSL, Valhalla, Sonovox ou Waves incorporados ou licenciados neste pacote.
AU e AAX nao sao entregues nesta versao Windows.

## Referencias tecnicas
JUCE 8.0.4: https://github.com/juce-framework/JUCE/tree/8.0.4
Oversampling: https://docs.juce.com/master/classjuce_1_1dsp_1_1Oversampling.html
Crossovers: https://docs.juce.com/master/classjuce_1_1dsp_1_1LinkwitzRileyFilter.html
Loudness: https://www.itu.int/rec/R-REC-BS.1770
