# Melhorias de processamento e presets do usuário

## Processamento

- O compressor agora usa tempos separados de ataque e recuperação, calculados pela taxa de amostragem.
- O expander/gate também usa ataque e recuperação separados para reduzir cortes bruscos no início e no fim das palavras.
- A proteção de pico passou de corte rígido para uma curva de saturação suave, preservando melhor os transientes quando a saída fica alta.
- O reverb deixou de recalcular os mesmos coeficientes de filtro e parâmetros a cada amostra; os valores só são atualizados quando os controles mudam.

## Presets pessoais

- `SALVAR +` grava uma cópia de todos os parâmetros do plugin com um nome escolhido pelo usuário.
- Os presets pessoais aparecem no menu de presets de cada aba e podem ser aplicados ou excluídos.
- O banco é incluído no estado serializado do plugin. Ele acompanha a instância quando o host salva o projeto ou quando o usuário salva um preset pelo gerenciador do host.
- O plugin não grava esse banco em AppData, Program Files nem altera o próprio DLL/VST3.

**Importante:** como os hosts guardam o estado do plugin junto ao projeto ou ao arquivo de preset do próprio host, é necessário salvar o projeto/preset no Sound Forge, Samplitude ou REAPER para manter alterações após fechar a sessão. Para levar os presets a outro projeto ou computador, salve e carregue um preset do plugin pelo gerenciador do host.

## Estado da validação

As alterações foram revisadas no código e `git diff --check` não apontou erros de whitespace. A compilação não foi executada neste ambiente porque o CMake não está instalado. Não instalar ou distribuir esta revisão como versão final antes de compilar e validar o plugin nos hosts Windows suportados.
