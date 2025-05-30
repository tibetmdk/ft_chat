#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

#define WIDTH 640
#define HEIGHT 480
#define MAX_MESSAGES 100
#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 12345

int sockfd;
char *messages[MAX_MESSAGES];
int message_count = 0;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
char *username;

// Gelen mesajları dinleyen thread
void *receive_messages(void *arg) {
	(void)arg;
    char buffer[1024];
    while (1) {
        int bytes = recv(sockfd, buffer, sizeof(buffer) - 1, 0);
        if (bytes <= 0) {
            printf("Sunucu bağlantısı kesildi.\n");
            close(sockfd);
            exit(1);
        }
        buffer[bytes] = '\0';

        pthread_mutex_lock(&lock);
        if (message_count < MAX_MESSAGES) {
            messages[message_count++] = strdup(buffer);
        }
        pthread_mutex_unlock(&lock);
    }
    return NULL;
}

int main() {
    // Kullanıcı adını al
    username = getenv("USER");
    if (!username)
        username = "Anonim";

    // Sunucuya bağlan
    struct sockaddr_in serv_addr;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Socket oluşturulamadı");
        return 1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(SERVER_PORT);
    inet_pton(AF_INET, SERVER_IP, &serv_addr.sin_addr);

    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("Sunucuya bağlanılamadı");
        return 1;
    }

    // SDL başlat
    SDL_Init(SDL_INIT_VIDEO);
    if (TTF_Init() == -1) {
        SDL_Log("TTF başlatılamadı: %s", TTF_GetError());
        return 1;
    }

    SDL_Window *window = SDL_CreateWindow("FT_CHAT",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        WIDTH, HEIGHT, SDL_WINDOW_SHOWN);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    TTF_Font *font = TTF_OpenFont("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf", 24);
    if (!font) {
        SDL_Log("Font yüklenemedi: %s", TTF_GetError());
        return 1;
    }

    // Mesaj dinleyici thread'i başlat
    pthread_t recv_thread;
    pthread_create(&recv_thread, NULL, receive_messages, NULL);

    char input[256] = "";
    SDL_Event event;
    SDL_StartTextInput();
    int running = 1;

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT)
                running = 0;

            else if (event.type == SDL_TEXTINPUT) {
                if (strlen(input) + strlen(event.text.text) < sizeof(input) - 1)
                    strcat(input, event.text.text);
            }

            else if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_BACKSPACE) {
                    int len = strlen(input);
                    if (len > 0)
                        input[len - 1] = '\0';
                } else if (event.key.keysym.sym == SDLK_RETURN) {
                    if (strlen(input) > 0) {
                        char full[512];
                        snprintf(full, sizeof(full), "%s: %s", username, input);
                        send(sockfd, full, strlen(full), 0);

                        pthread_mutex_lock(&lock);
                        if (message_count < MAX_MESSAGES)
                            messages[message_count++] = strdup(full);
                        pthread_mutex_unlock(&lock);

                        input[0] = '\0';
                    }
                }
            }
        }

        // Arayüzü temizle
        SDL_SetRenderDrawColor(renderer, 30, 30, 30, 255);
        SDL_RenderClear(renderer);

        SDL_Color color = {255, 255, 255, 255};

        // Mesajları ekrana yaz
        pthread_mutex_lock(&lock);
        for (int i = 0; i < message_count; i++) {
            SDL_Surface *surf = TTF_RenderText_Blended(font, messages[i], color);
            SDL_Texture *tex = SDL_CreateTextureFromSurface(renderer, surf);

            SDL_Rect rect = {20, 20 + i * 30, surf->w, surf->h};

            // Eğer kendi mesajınsa sağa hizala
            if (strncmp(messages[i], username, strlen(username)) == 0) {
                rect.x = WIDTH - surf->w - 20;
            }

            SDL_RenderCopy(renderer, tex, NULL, &rect);
            SDL_FreeSurface(surf);
            SDL_DestroyTexture(tex);
        }
        pthread_mutex_unlock(&lock);

        // Aktif input'u en alta yaz
        if (strlen(input) > 0) {
            char preview[512];
            snprintf(preview, sizeof(preview), "%s: %s", username, input);

            SDL_Surface *surf = TTF_RenderText_Blended(font, preview, color);
            SDL_Texture *tex = SDL_CreateTextureFromSurface(renderer, surf);
            SDL_Rect rect = {20, HEIGHT - 40, surf->w, surf->h};
            rect.x = WIDTH - surf->w - 20;
            SDL_RenderCopy(renderer, tex, NULL, &rect);
            SDL_FreeSurface(surf);
            SDL_DestroyTexture(tex);
        }

        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }

    // Temizlik
    SDL_StopTextInput();
    TTF_CloseFont(font);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();

    for (int i = 0; i < message_count; i++)
        free(messages[i]);

    close(sockfd);
    return 0;
}