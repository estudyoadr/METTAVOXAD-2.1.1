# Compatibilidade e build 2.1.0

| Uso | Arquitetura | Formato |
|---|---|---|
| Sound Forge 8, processo de 32 bits | x86 | DLL VST legada METTAVOXAD21.dll |
| Outros hosts de 32 bits | x86 | DLL legada ou VST3 se suportado pelo host |
| Hosts de 64 bits | x64 | DLL legada ou VST3 se suportado pelo host |
| Linux x64 | x64 | VST3 |

A arquitetura acompanha o host, não apenas o Windows. O adaptador VST legado é código original neste pacote e não depende de um SDK VST2 externo. O processamento/editor é compartilhado com VST3. Não há bridge x86/x64.

JUCE fixado em 8.0.4, C++17, MSVC com runtime estático. Os instaladores configuram Windows 7 SP1 como mínimo; a compilação local recomendada é feita em Windows 10/11 x64. Esse mínimo é uma configuração, não uma certificação de compatibilidade. Sound Forge 8 e o instalador Windows precisam de teste real no destino.

Veja PASSO_A_PASSO_WINDOWS.md para GitHub, PowerShell, atualização e assinatura. A pasta .github está incluída no ZIP e o arquivo COPIAR_WORKFLOW_GITHUB.txt permite criar o workflow pela interface do GitHub.


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
