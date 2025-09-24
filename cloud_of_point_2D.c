/** @file cloud_of_point_2D.c
* @author G. Maxime
* @details Nuage de points 2D interactif avec OpenGL et GLFW
* @date 2025-05-07
*
* Commande pour compiler avec GCC :
* @code 
* gcc -Wall cloud_of_point_2D.c -lglfw -lGL -lX11 -lm -o cloud_of_point_2D
* @endcode
*
* Commande pour executer le programme : 
* @code
* ./cloud_of_point_2D
* @endcode
*/

/* Includes */
#include <GLFW/glfw3.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

/* Constantes */
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define NUM_POINTS 1000
#define POINT_SIZE 0.007
#define HIGHLIGHT_RADIUS 0.03
#define CLICK_RADIUS 0.1

/* Structure Point(x,y,v_x,v_y) */
typedef struct {
    float x, y;
    float v_x, v_y;
} Point;

/* Variables Global */
Point points[NUM_POINTS];
Point virtual_point;
int k = 2;
int mouse_pressed = 0;
int w_width, w_height;
double speed=1./50.;
double last_update, next_update;

/* Génération des points */
void generate_points() {
    for (int i = 0; i < NUM_POINTS; i++) {
        points[i].x = (float)rand() / RAND_MAX * 2.0f - 1.0f;
        points[i].y = (float)rand() / RAND_MAX * 2.0f - 1.0f;
        points[i].v_x = (float)rand() / RAND_MAX * 0.01f - 0.005f;
        points[i].v_y = (float)rand() / RAND_MAX * 0.01f - 0.005f;              
    }
}

/* Dessin d'un point */
void draw_point(Point p, float size) {
    float x_ratio = w_width / w_height;
    float size_x = size * x_ratio;
    glBegin(GL_LINES);
    glVertex2f(p.x - size_x, p.y);
    glVertex2f(p.x + size_x, p.y);
    glVertex2f(p.x, p.y - size);
    glVertex2f(p.x, p.y + size);
    glEnd();
}

/* Dessin de l'ensemble des points */
void draw_points() {
    glColor3f(1.0f, 0.0f, 0.0f);
    for (int i = 0; i < NUM_POINTS; i++) {
        draw_point(points[i], POINT_SIZE);
    }
}

/* Dessin d'un cercle */
void draw_circle(Point center, float radius) {
    glBegin(GL_LINE_LOOP);
    glColor3f(1.0f, 1.0f, 1.0f);
    for (float i = 0.0 ; i < 360.0; i += 0.5) {
        float theta = i * M_PI / 180.0f;
        float x = radius * cosf(theta);
        float y = radius * sinf(theta);
        glVertex2f(center.x + x, center.y + y);
    }
    glEnd();
}

/* Calcul de la distance entre deux points */
float distance(Point a, Point b) {
    return sqrtf((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y));
}

/* Renvoie la liste des k points les plus proche du point centre */
void find_nearest_points(Point center, int k, Point* nearest_points, float* max_dist) {
    float dists[NUM_POINTS];
    for (int i = 0; i < NUM_POINTS; i++) {
        dists[i] = distance(center, points[i]);
    }

    for (int i = 0; i < k; i++) {
        int min_index = -1;
        for (int j = 0; j < NUM_POINTS; j++) {
            if (dists[j] >= 0 && (min_index == -1 || dists[j] < dists[min_index])) {
                min_index = j;
            }
        }
        nearest_points[i] = points[min_index];
        *max_dist = dists[min_index];
        dists[min_index] = -1; // Marquer comme utilisé
    }
}

/* Callbacks */
/* Position du curseur */
void cursor_position_callback(GLFWwindow* window, double xpos, double ypos) {
    if (mouse_pressed) {
        int width, height;
        glfwGetWindowSize(window, &width, &height);
        float aspect_ratio = (float)width / (float)height;
        if (aspect_ratio > 1.0) {
            virtual_point.x = (float)xpos / width * aspect_ratio * 2.0f - aspect_ratio;
            virtual_point.y = 1.0f - (float)ypos / height * 2.0f;
        } else if (aspect_ratio < 1.0) {
            virtual_point.x = (float)xpos / width * 2.0f - 1.0f;
            virtual_point.y = 1.0f/aspect_ratio - (float)ypos / height * 2.0f / aspect_ratio;
        } else if (aspect_ratio == 1.0) {
            virtual_point.x = (float)xpos / width * 2.0f - 1.0f;
            virtual_point.y = 1.0f - (float)ypos / height * 2.0f;
        }
    }
}

/* Action sur des touches du clavier */
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS) {
        if (key == GLFW_KEY_KP_ADD) {
            k++;
        } else if (key == GLFW_KEY_KP_SUBTRACT) {
            k--;
            if (k < 1) k = 1;
        }
    }
}

/* Action sur des boutons de la souris */
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        if (action == GLFW_PRESS) {
            mouse_pressed = 1;
            double xpos, ypos;
            glfwGetCursorPos(window, &xpos, &ypos);
            cursor_position_callback(window, xpos, ypos);
        } else if (action == GLFW_RELEASE) {
            mouse_pressed = 0;
        }
    }
}

void point_update(Point* p) {
    p->x += p->v_x;
    p->y += p->v_y;
    if (p->x > 1.0f || p->x < -1.0f) {
        p->v_x = -p->v_x;
    }
    if (p->y > 1.0f || p->y < -1.0f) {
        p->v_y = -p->v_y;
    }
}

void redraw_points(GLFWwindow* window) {
    glClear(GL_COLOR_BUFFER_BIT);
    glColor3f(1.0f, 0.0f, 0.0f);
    for (int i = 0; i < NUM_POINTS; i++) {
        point_update(&points[i]);
    }
    draw_points();
}

/* Callback pour la taille de la fenêtre */
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    float aspect_ratio = (float)width / (float)height;
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    if (aspect_ratio > 1.0) {
        glOrtho(-aspect_ratio, aspect_ratio, -1.0, 1.0, -1.0, 1.0);
    } else if (aspect_ratio < 1.0) {
        glOrtho(-1.0, 1.0, -(1.0/aspect_ratio), (1.0/aspect_ratio), -1.0, 1.0);
    } else if (aspect_ratio == 1.0) {
        glOrtho(-1.0, 1.0, -1.0, 1.0, -1.0, 1.0);
    }
    glMatrixMode(GL_MODELVIEW);
}

/* Main */
int main(void) {
    if (!glfwInit()) {
        return -1;
    }

    last_update=glfwGetTime();
    next_update=glfwGetTime()+speed;

    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    glfwGetMonitorWorkarea(monitor, NULL, NULL, &w_width, &w_height);
    // Création de la fenêtre
    GLFWwindow* window;
    window = glfwCreateWindow(w_width, w_height, "cloud_of_points.exe", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSwapBuffers(window);
    glfwMakeContextCurrent(window);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetCursorPosCallback(window, cursor_position_callback);
    glfwSetKeyCallback(window, key_callback);

    float aspect_ratio = (float)w_width / (float)w_height;
    glViewport(0, 0, w_width, w_height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-aspect_ratio, aspect_ratio, -1.0, 1.0, -1.0, 1.0);

    generate_points();

    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT);

        draw_points();

        if (virtual_point.x != 0 || virtual_point.y != 0) {
            Point nearest_points[k];
            float max_dist = 0;
            find_nearest_points(virtual_point, k, nearest_points, &max_dist);

            glColor3f(0.0f, 1.0f, 0.0f);
            for (int i = 0; i < k; i++) {
                draw_point(nearest_points[i], POINT_SIZE);
            }

            draw_circle(virtual_point, max_dist);

            // Dessiner des lignes entre le point virtuel et les k points les plus proches
            glBegin(GL_LINES);
            for (int i = 0; i < k; i++) {
                glVertex2f(virtual_point.x, virtual_point.y);
                glVertex2f(nearest_points[i].x, nearest_points[i].y);
            }
            glEnd();
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
        double now = glfwGetTime();
        if(now>=next_update){
            last_update=now;
            next_update=last_update+speed;
            redraw_points(window);
        }
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
