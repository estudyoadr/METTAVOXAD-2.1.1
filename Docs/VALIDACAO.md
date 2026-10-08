# Validacao da entrega 2.1.1

Conferencia estatica realizada em 08/10/2026:
- Os quatro bancos de seis presets sao identicos, caractere por caractere, aos bancos da 2.1.0.
- Os programas globais e o preset Default foram preservados.
- Workflow YAML lido corretamente; matriz Windows x86/x64 presente.
- COPIAR_WORKFLOW_GITHUB.txt e identico ao workflow incluido.

Compilacao, DSP, interface e instaladores **ainda nao executados para esta versao**.
O ambiente desta entrega nao possui cmake/JUCE para compilacao local.
Os testes existentes foram ajustados para capturar a interface em 660 x 330 e verificar
os controles tambem em 990 x 495. O workflow executa os testes antes de publicar
os artifacts Windows. O teste de upgrade verifica substituicao da DLL, atualizacao
do registro para 2.1.1 e remocao de arquivo VST3 obsoleto da fixture 2.1.0.
A fixture usa a DLL da compilacao atual para simular a instalacao antiga; nao
representa um teste com o binario real distribuido da 2.1.0.

Os resultados e screenshots em Historico_2.1.0 sao exclusivamente da versao anterior.
A compatibilidade real com Sound Forge precisa ser confirmada pelo usuario.
