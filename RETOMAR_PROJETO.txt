METTAVOXAD 2.1.1 — INSTALADORES WINDOWS

24 presets originais preservados: seis em cada uma das quatro abas.

INSTALACAO
1. Feche o Sound Forge e os outros programas de audio.
2. Use x86 para Sound Forge de 32 bits (incluindo Sound Forge 8).
   Use x64 para hosts de 64 bits. A arquitetura do host define a escolha,
   mesmo quando o Windows e de 64 bits.
3. Execute o Setup.exe correspondente. Nao e necessario compilar nada.
4. Abra o host e atualize a busca por plugins. Procure METTAVOXAD21.
5. Se necessario, inclua o diretorio VST2 mostrado pelo instalador na busca do host.

VALIDACAO CONCLUIDA EM 08/10/2026
Compilacao DLL/VST3, carregamento legado, audio/estado, 24 presets,
16 combinacoes de modulos, Autotune, Voz Robo, interface 660x330 e 990x495,
geracao EXE e atualizacao do instalador: PASS em x86 e x64.
Capturas reais e logs incluidos neste pacote.
A fixture de atualizacao usa a DLL atual para simular a versao antiga;
nao e uma execucao do binario historico real da 2.1.0.
A verificacao em Sound Forge/Samplitude/Reaper no computador do usuario
continua necessaria. Os testes automaticos nao substituem essa verificacao.

CORRECOES FINAIS
- Workflow renomeado para .yml sem ponto extra.
- DLL x86 exporta VSTPluginMain e main usando LegacyVst.def.
- Fontes rastreados no GitHub tem prioridade sobre o ZIP antigo.
- Nenhum arquivo Source foi alterado; efeitos, presets e identidade preservados.

GITHUB
https://github.com/estudyoadr/METTAVOXAD-2.1.1
Commit validado: 3bcaa43acf13490e22b66c6e63f2416c2b23e416
Testes: https://github.com/estudyoadr/METTAVOXAD-2.1.1/actions/runs/37853256436
