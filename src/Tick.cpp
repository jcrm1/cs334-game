

#include "Tick.hpp"


// GENERAL DISPATCH - MACOS
// AI-generated code, replace with actual logic
#if defined(__APPLE__)
#include <dispatch/dispatch.h>

void mesh_append_triangle(const Triangle* tri) {
    if (vertex_count + 3 > MAX_VERTICES)
        return;

    cpu_vertices[vertex_count + 0] = tri->v0;
    cpu_vertices[vertex_count + 1] = tri->v1;
    cpu_vertices[vertex_count + 2] = tri->v2;

    /* Upload only new data */
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferSubData(
        GL_ARRAY_BUFFER,
        (vertex_count) * sizeof(Vertex),
        3 * sizeof(Vertex),
        &cpu_vertices[vertex_count]
    );

    vertex_count += 3;
}


void append_triangle_to_mesh(void* context) {
    Triangle* tri = (Triangle*)context;

    mesh_append_triangle(tri);

    free(tri);
}

void scheduler_run(void* context) {
    SchedulerContext* ctx = (SchedulerContext*)context;

    double lastDelay = 0.0;

    for (size_t i = 0; i < ctx->count; ++i) {
        if (*ctx->cancelled) break;

        TimedTriangle* item = &ctx->schedule[i];
        double delta = item->delay - lastDelay;

        if (delta > 0.0) {
            usleep((useconds_t)(delta * 1e6));
        }

        /* Copy triangle for async ownership */
        Triangle* triCopy = malloc(sizeof(Triangle));
        *triCopy = item->triangle;

        dispatch_async_f(
            dispatch_get_main_queue(),
            triCopy,
            append_triangle_to_mesh
        );

        lastDelay = item->delay;
    }

    free(ctx);  // cleanup scheduler context
}

void start_mesh_schedule(TimedTriangle* schedule, size_t count) {
    static volatile bool cancelled = false;

    SchedulerContext* ctx = malloc(sizeof(SchedulerContext));
    ctx->schedule  = schedule;
    ctx->count     = count;
    ctx->cancelled = &cancelled;

    dispatch_queue_t queue =
        dispatch_queue_create("mesh.scheduler", DISPATCH_QUEUE_SERIAL);

    dispatch_async_f(queue, ctx, scheduler_run);
}

#endif


// Polling - Windows
// AI-generated code, replace with actual logic

#if defined(_WIN32)
#include <windows.h>

void startTimer() {

    LARGE_INTEGER freq, last, now;
    QueryPerformanceFrequency(&freq);
    QueryPerformanceCounter(&last);

    const double interval = 1.0 / 500000.0; // 500 kHz

    while (true) {
        QueryPerformanceCounter(&now);
        double elapsed = (double)(now.QuadPart - last.QuadPart) / freq.QuadPart;

        if (elapsed >= interval) {
            last = now;
            // "interrupt" logic here
        }
    }
}
#endif