// Henon Map — Butterfly Attractor
// x_{n+1} = 1 - a·x_n² + y_n
// y_{n+1} = b·x_n
// Classic chaos: tiny init change → totally different trajectory
// Build: g++ -O2 -std=c++17 main.cpp -o henon

#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <vector>
#include <string>

int main(int argc, char* argv[]) {
    const double a = 1.4;
    const double b = 0.3;
    const int ITER = 200000;
    const int TRANS = 5000;

    std::string save_path = "henon.ppm";
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--save" && i + 1 < argc) save_path = argv[++i];
    }

    const int W = 900, H = 600;
    std::vector<int> img(W * H, 0);

    // Trajectory 1: x0 = 0.1
    double x = 0.1, y = 0.1;
    for (int i = 0; i < TRANS + ITER; ++i) {
        double nx = 1.0 - a * x * x + y;
        double ny = b * x;
        x = nx; y = ny;
        if (i < TRANS) continue;

        int px = int((x + 1.5) / 3.0 * W);
        int py = int((y + 0.5) / 1.5 * H);
        if (px >= 0 && px < W && py >= 0 && py < H)
            img[py * W + px] = std::min(img[py * W + px] + 1, 255);
    }

    // Trajectory 2: x0 = 0.101  (diff = 0.001 — the butterfly wing flap)
    x = 0.101; y = 0.1;
    for (int i = 0; i < TRANS + ITER; ++i) {
        double nx = 1.0 - a * x * x + y;
        double ny = b * x;
        x = nx; y = ny;
        if (i < TRANS) continue;

        int px = int((x + 1.5) / 3.0 * W);
        int py = int((y + 0.5) / 1.5 * H);
        if (px >= 0 && px < W && py >= 0 && py < H)
            img[py * W + px] = std::min(img[py * W + px] + 3, 255);  // brighter for traj2
    }

    FILE* fp = fopen(save_path.c_str(), "w");
    fprintf(fp, "P3\n%d %d\n255\n", W, H);
    for (int i = 0; i < W * H; ++i) {
        int v = img[i];
        // Gold/orange for dense regions
        int r = std::min(v * 2, 255);
        int g = std::min(v, 255);
        int b = std::min(v / 4, 255);
        fprintf(fp, "%d %d %d\n", r, g, b);
    }
    fclose(fp);
    printf("Henon butterfly saved: %s (%dx%d, %d iters)\n", save_path.c_str(), W, H, ITER * 2);

    // ASCII preview
    const int AW = 60, AH = 30;
    int density[AW * AH] = {0};
    x = 0.1; y = 0.1;
    for (int i = 0; i < TRANS + ITER; ++i) {
        double nx = 1.0 - a * x * x + y;
        double ny = b * x;
        x = nx; y = ny;
        if (i < TRANS) continue;
        int px = int((x + 1.5) / 3.0 * AW);
        int py = int((y + 0.5) / 1.5 * AH);
        if (px >= 0 && px < AW && py >= 0 && py < AH)
            density[py * AW + px]++;
    }

    printf("\nASCII preview:\n");
    const char* chars = " .:-=+*#@";
    for (int y = 0; y < AH; ++y) {
        for (int x = 0; x < AW; ++x) {
            int v = density[y * AW + x];
            int idx = v == 0 ? 0 : std::min(int(8.0 * std::log1p(v) / std::log1p(1000.0) + 1), 8);
            putchar(chars[idx]);
        }
        putchar('\n');
    }

    // Lyapunov exponent: λ = (1/N) Σ log(||J(x_n)||_2)
    // J = [[-2ax, 1], [b, 0]], spectral radius ≈ sqrt(2|a||x| + 1)
    double lam_sum = 0.0;
    x = 0.1; y = 0.1;
    for (int i = 0; i < 50000; ++i) {
        double J00 = -2.0 * a * x, J01 = 1.0, J10 = b, J11 = 0.0;
        double norm = std::sqrt(J00*J00 + J01*J01 + J10*J10 + J11*J11);
        lam_sum += std::log(std::max(0.001, norm));
        double nx = 1.0 - a * x * x + y;
        double ny = b * x;
        x = nx; y = ny;
    }
    double lam = lam_sum / 50000.0;
    printf("\nMax Lyapunov exponent λ ≈ %.4f (positive = chaos)\n", lam);

    // Butterfly effect: two trajectories with tiny initial difference
    double x1 = 0.1, y1 = 0.1;
    double x2 = 0.101, y2 = 0.1;  // diff = 0.001
    printf("\nButterfly effect (initial diff = 0.001):\n");
    printf("%-6s %-12s %-12s %-12s\n", "n", "x1", "x2", "divergence");
    for (int n = 0; n <= 30; ++n) {
        if (n > 0) {
            double nx1 = 1.0 - a * x1 * x1 + y1; double ny1 = b * x1;
            double nx2 = 1.0 - a * x2 * x2 + y2; double ny2 = b * x2;
            x1 = nx1; y1 = ny1; x2 = nx2; y2 = ny2;
        }
        double div = std::abs(x1 - x2);
        if (n <= 5 || n == 10 || n == 20 || n == 30)
            printf("%-6d %.12f %.12f %.12f\n", n, x1, x2, div);
    }
    printf("→ After 30 iterations the trajectories are completely unrelated.\n");

    return 0;
}