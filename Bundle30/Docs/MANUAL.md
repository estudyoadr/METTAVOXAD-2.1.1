# MettaVoxAD 3.0.2 Bundle — retoque vocal e identidade Metta

Sete plugins independentes, DLL legada e VST3, Windows x86 e x64.
Instale a arquitetura do HOST: Sound Forge 8 usa x86, mesmo em Windows 64 bits.
O instalador do bundle nao substitui nem remove MettaVoxAD 2.1.1.

## Cadeia sugerida
MettaVoxAD Pure > MettaVoxAD Vocal > MettaVoxAD Impact > MettaVoxAD Morph > MettaVoxAD Orbit > MettaVoxAD Velvet > MettaVoxAD Apex.
Nao e preciso usar todos: comece MettaVoxAD Pure, MettaVoxAD Vocal e Ceiling. Acrescente os demais conforme a locucao.
A captacao, a voz, o instrumental e a escuta determinam o resultado final. Referencia de radio imaging americana e direcao artistica, nao equivalencia comprovada a uma producao ReelWorld.

## Operacao
Cada plugin tem Default e seis presets, entrada/saida em dB, bypass com latencia compensada, parametros automatizaveis e estado salvo pelo host. Sao 42 presets novos no bundle. Os 24 presets originais continuam no 2.1.1.

- **MettaVoxAD Pure:** atenuacao espectral WOLA/FFT de 512 pontos, ajuste manual de piso, de-esser de banda, controle de baixas/plosivas e interpolacao de cliques isolados. Ajuste Piso ao ruido real. Reducao excessiva pode criar artefatos. Nao remove toda reverberacao ambiente nem todos estalos de saliva. Gain Staging e opcional. O De-Thump usa filtro/detector de baixas; nao e separacao espectral de plosivas.
- **MettaVoxAD Vocal:** HPF e controle dinamico de graves medios; compressores rapido e optico em serie; saturacao de tres bandas; coesao e sala curta com pre-delay filtrado e ducking. Afinacao monofonica opcional, derivada do 2.1.1, desligada nos presets de locucao. A tecnica granular nao preserva formantes como um editor de afinacao offline e deve ser avaliada em vozes cantadas.
- **MettaVoxAD Impact:** contraste entre envelopes rapido/lento, ataque e sustain, punch de banda, expansao suave e clipping oversampled. Reduza Attack quando a voz ficar dura.
- **MettaVoxAD Morph:** subgrave rastreado na faixa de 40 a 90 Hz, portadora vocoder PolyBLEP seguindo pitch, formantes por remapeamento das bandas, oito estagios all-pass e micro-pitch. XY controla Aura (X), Synth e Sub (Y). Intelligibility reduz wet ate 35% nos eventos de alta frequencia. Sub nao e gerado no silencio. Aura e phaser IIR, nao fase linear. O preset Radio Flyer usa modulacao e formantes; nao executa uma automacao de subida exata de uma oitava.
- **MettaVoxAD Orbit:** eco cruzado filtrado e feedback limitado; micro-pitch; ducking. Ritmo 0 = milissegundos; 1 = esquerda 1/8 pontilhado/direita 1/4; 2 = esquerda 1/4 pontilhado/direita 1/4; 3 = esquerda 1/8/direita 1/4. Usa BPM do host quando disponivel, senao 120 BPM.
- **MettaVoxAD Velvet:** saturacao por bandas e shelf Air com protecao de sibilancia. Nao transforma fisicamente um microfone em outro. IPS muda a intensidade da saturacao: 15 ips mais denso, 30 ips mais limpo; nao e emulacao fisica completa de fita.
- **MettaVoxAD Apex:** caminho oversampled 4x, lookahead 5 ms, detector stereo, reserva de pico para a reconstrucao, medicao independente 8x, LUFS momentaneo/curto/integrado K-weighted com gates absoluto/relativo. O integrado retém ate uma hora e reinicia quando o host prepara o processador. Nao ha certificacao de conformidade nem medicao de codecs. Alvo LUFS orienta o Auto Level opcional e nao garante o loudness integrado final. Dry/Wet muda drive/densidade antes da limitacao; a protecao continua ativa. Ganho positivo de saida e limitado a 0 dB para preservar o teto.

## Perfis de entrega
Radio Wall (-8 LUFS, -0.3 dBTP) e preset criativo agressivo, nao norma universal de radio. Streaming e TV sao pontos de partida: confira a especificacao real de quem recebe o arquivo. Normalizacao das plataformas e controlada por elas.

## Engenharia e limites
Saturacao/clipping/sintese usam oversampling FIR 4x. Crossovers Linkwitz-Riley em 200 Hz e 4 kHz, com alinhamento all-pass da banda grave no somado interno. Nao ha garantias de ausencia absoluta de aliasing ou de resultado artistico; testes e audicao sao necessarios.
Controles foram implementados localmente. Nao ha Pro-Q, Melodyne, 1176, LA-2A, Decapitator, SSL, Valhalla, Sonovox ou Waves incorporados ou licenciados neste pacote.
AU e AAX nao sao entregues nesta versao Windows.

## Referencias tecnicas
JUCE 7.0.12: https://github.com/juce-framework/JUCE/tree/7.0.12
Oversampling: https://docs.juce.com/master/classjuce_1_1dsp_1_1Oversampling.html
Crossovers: https://docs.juce.com/master/classjuce_1_1dsp_1_1LinkwitzRileyFilter.html
Loudness: https://www.itu.int/rec/R-REC-BS.1770


## Vocal 3.0.2 — perfis Corpo 87 e Tubo 12
O menu de timbre fica a direita do menu de presets. Sao perfis tonais originais,
nao respostas medidas de AKG/Neumann nem modelos fisicos completos de microfones.
Corpo 87: reforco largo em 105 Hz, presenca em 3.4 kHz e ar em 12 kHz.
Tubo 12: mais corpo em 105 Hz, presenca mais baixa em 2.8 kHz e harmônicos
assimétricos suaves, com de-ess interno adaptativo de ate 3 dB.
Ambos usam nivelamento optico inspirado na arquitetura do VT-737sp:
soft knee de 6 dB, ataque 10/15 ms, release 140/180 ms com recuperacao
mais lenta conforme a reducao acumulada; peak catcher de 2 ms.
Compressao macro 65: threshold -23.2 dBFS, ratio 4.275:1, intensidade 65%,
makeup 3.25 dB. O valor dBFS e referencia digital propria: nao corresponde ao
threshold dBu do hardware. Ajuste entrada observando GR: 3 a 6 dB nas frases
fortes e um ponto de partida, nunca preset universal.
A inspiracao e tecnica, nao emulacao licenciada nem configuracao pessoal do Cezaronline.

Presets Metta Presence 87, Metta Tube Authority, Metta Velvet Commercial,
Metta Crystal Narration, Metta Pop Polish e Metta Dense Promo substituem os seis
presets do modulo vocal do bundle; continua havendo 42 presets no total.
Presence 87 e o primeiro ponto de partida para locucao com clareza e peso;
Tube Authority para corpo maior. Sala permanece discreta, 1 a 3% nos presets
falados. Evite somar ganhos exagerados em Vocal, Velvet e Apex.
O perfil Original 3.0 mantem o motor anterior. Estados salvos na versao 3.0 sem
parametro de timbre recebem automaticamente Original 3.0. IDs internos permanecem
iguais, para preservar a identificacao em projetos existentes. Alguns hosts podem
precisar refazer a busca depois que os arquivos mudarem de nome.

## Atualizacao de nomes
Pure = limpeza; Vocal = tonalidade/dinamica; Impact = transientes;
Morph = efeitos vocais; Orbit = delay; Velvet = harmonicos; Apex = limiter.
Os arquivos e nomes visiveis agora usam MettaVoxAD. O instalador remove somente
os sete nomes antigos MV3 da pasta do bundle, preservando o 2.1.1.

## Referencia fornecida
https://vimeo.com/670671432 — VOICE SESSION CHARLIE PUFF, cezaronline.
Foi possivel verificar titulo/autor; o acesso ao fluxo de audio do Vimeo retornou
403. Nao foi feita medicao nem audicao do som do video. Nao foram encontrados
ajustes publicos do compressor usado nessa sessao. A calibracao com esse audio
continua dependente de uma amostra acessivel.
Arquitetura oficial VT-737sp: https://www.avalondesign.com/vt-737-sp


## Correcao de compatibilidade — 09/10/2026
Base JUCE 7.0.12 com interface Windows software/GDI, sem renderizacao obrigatoria
por Direct2D moderno. O adaptador legado consulta tamanho sem criar editor;
editor e timers passam a usar a thread em que o host abre a janela. Contexto
DPI herdado temporariamente do HWND pai, sem alterar o processo do host.
Parent HWND invalido e rejeitado. EffEditIdle executa repintura pendente.
Os testes agora carregam DLL em thread de scanner, abrem/fecham/reabrem a janela
nativa em tres contextos de awareness DPI e validam pixels pintados. VST3 passa
por scanner, factory, audio e editor nativo. Arquitetura PE/imports e runtime
estatico Visual C++ sao auditados na compilacao.
O alvo de compatibilidade inclui Windows 7 SP1/8.1/10/11; a execucao automatizada
usa Windows Server 2022. Nao equivale a testes presenciais em cada SO/GPU/DAW.

VST2/DLL nao tem pasta universal descoberta por todos os hosts. Na instalacao,
escolha a pasta ja pesquisada pelo programa de audio, ou adicione a pasta
instalada a lista VST do host e refaca a busca. A pagina de destino fica visivel
mesmo em atualizacoes. VST3 usa Common Files/VST3 da arquitetura correspondente.
Sound Forge 8 requer a DLL x86; nao le VST3 nem carrega DLL x64.
As duas arquiteturas sao separadas; escolher pela arquitetura do host, nao do SO.
