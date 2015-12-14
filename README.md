# chat_IRC2

O Internet Relay Chat é um protocolo de comunicação via internet utilizado como  
bate­papo e troca de arquivos permitindo troca de informações em grupo ou de forma privada,  
ele suporta conversas por texto, em canais. O protocolo é aberto e utiliza conexão via TCP. Um
servidor de IRC pode ser ligado a outros servidores de forma a expandir a rede de IRC.
Está é uma aplicação em rede que implementa o protocolo IRC­2, possibilitando a troca de mensagem num sistema
*chat* e que possua um comando no qual permita um usuário bloquear outros.

## Compilar e executar

- O programa deve ser compilado com o compilador GCC através dos seguintes comandos:
   ```
   gcc ­o server server.c hash.c list.c 
   gcc ­o client client.c
   ```

- A execução do programa deve ser executada com os seguintes parâmetros:
   - Servidor:
   ```
   ./server <porta>
   ```

   - Cliente:
   ```
   ./client <ip/nome> <porta> <usuario>  
   ```

## Comandos suportados

1. **NICK <nick>**: apelido do cliente. Utilizado com identificador do cliente para 
comunicação 
2. **NEW <nick> <mensagem>**: formato de mensagem enviada do servidor para clientes. 
3. **POST <mensagem>**: envio de mensagens do cliente para o servidor, tamanho máximo da mensagem de 500 caracteres. 
4. **MUTE <nick>**: indicativo do cliente para o servidor que aquele cliente não deseja receber mensagens do cliente identificado (pelo nick)
5. **UNMUTE <nick>**:  indicativo do cliente para o servidor que aquele cliente deseja “desbloquear” o usuário identificado pelo nick para que eles possam se comunicar. 
6. **CLOSE**: fechamento da conexão com o servidor, encerrando a comunicação.
