#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <omp.h>

#define ROCK '*'
#define RABBIT 'R'
#define FOX 'F'
#define EMPTY ' '

typedef struct Data {
    char entity;
    int breeding_age;
    int starving_age;
    int last_move_gen;
    char current_entity;
} data;

typedef struct Position {
    int i;
    int j;
} position;

uint32_t ngenerations, M, N, nrock, nrab, rbreed, nfox, fbreed, fstarv, seed;
data **world;

float r4_uni(uint32_t *seed) {
    int seed_input, sseed;
    float r;

    seed_input = *seed;
    *seed ^= (*seed << 13);
    *seed ^= (*seed >> 17);
    *seed ^= (*seed << 5);
    sseed = *seed;
    r = 0.5 + 0.2328306e-09 * (seed_input + sseed);

    return r;
}

int data_empty(int i, int j) {
    if(world[i][j].current_entity == EMPTY) return 1;
    else return 0;
}

void insert_animal(int i, int j, char atype) {
    world[i][j].entity = atype;
    world[i][j].current_entity = atype;
}

int generate_element(int n, char atype, uint32_t *seed) {
    int i, j, k, cnt = 0;

    for(k = 0; k < n; k++) {
        i = M * r4_uni(seed);
        j = N * r4_uni(seed);
        if(data_empty(i, j)) {
            insert_animal(i, j, atype);
            cnt++;
        }
    }

    return cnt;
}

void reset_position(int i, int j) {
    world[i][j].entity = EMPTY;
    world[i][j].breeding_age = 0;
    world[i][j].starving_age = 0;
    world[i][j].last_move_gen = 0;
    world[i][j].current_entity = EMPTY;
}

void move_rabbit(int i, int j, int gen) {
    int cnt = 0, cell_number = (i * N) + j, new_rabbit = 0;
    position empty_neighbors[4], *p;

    world[i][j].breeding_age++;
    
    if(i - 1 >= 0 && world[i - 1][j].current_entity == EMPTY) {
        empty_neighbors[cnt].i = i - 1;
        empty_neighbors[cnt].j = j;
        cnt++;
    }
    if(j + 1 < N && world[i][j + 1].current_entity == EMPTY) {
        empty_neighbors[cnt].i = i;
        empty_neighbors[cnt].j = j + 1;
        cnt++;
    }
    if(i + 1 < M && world[i + 1][j].current_entity == EMPTY) {
        empty_neighbors[cnt].i = i + 1;
        empty_neighbors[cnt].j = j;
        cnt++;
    }
    if(j - 1 >= 0 && world[i][j - 1].current_entity == EMPTY) {
        empty_neighbors[cnt].i = i;
        empty_neighbors[cnt].j = j - 1;
        cnt++;
    }

    if(cnt == 0) return;
    p = &empty_neighbors[cell_number % cnt];

    if(world[i][j].breeding_age >= rbreed) {
        world[i][j].breeding_age = 0;
        new_rabbit = 1;
    }

    // Conflict resolution
    switch(world[p->i][p->j].entity) {
        case EMPTY:
            world[p->i][p->j].entity = world[i][j].entity;
            world[p->i][p->j].breeding_age = world[i][j].breeding_age;
            world[p->i][p->j].last_move_gen = gen;
            break;
        case RABBIT:
            if(world[i][j].breeding_age > world[p->i][p->j].breeding_age)
                world[p->i][p->j].breeding_age = world[i][j].breeding_age;
            break;
        case FOX:
            world[p->i][p->j].starving_age = 0;
            break;
    }

    // Breeding or not
    if(!new_rabbit) {
        reset_position(i, j);
    }
}

void move_fox(int i, int j, int gen) {
    int empty_cnt = 0, rabbit_cnt = 0, cell_number = (i * N) + j, new_fox = 0;
    position empty_neighbors[4], rabbit_neighbors[4], *p;

    world[i][j].breeding_age++;
    world[i][j].starving_age++;

    if(i - 1 >= 0) {
        if(world[i - 1][j].current_entity == EMPTY) {
            empty_neighbors[empty_cnt].i = i - 1;
            empty_neighbors[empty_cnt].j = j;
            empty_cnt++;
        }
        else if(world[i - 1][j].current_entity == RABBIT) {
            rabbit_neighbors[rabbit_cnt].i = i - 1;
            rabbit_neighbors[rabbit_cnt].j = j;
            rabbit_cnt++;
        }
    }
    if(j + 1 < N) {
        if(rabbit_cnt == 0 && world[i][j + 1].current_entity == EMPTY) {
            empty_neighbors[empty_cnt].i = i;
            empty_neighbors[empty_cnt].j = j + 1;
            empty_cnt++;
        }
        else if(world[i][j + 1].current_entity == RABBIT) {
            rabbit_neighbors[rabbit_cnt].i = i;
            rabbit_neighbors[rabbit_cnt].j = j + 1;
            rabbit_cnt++;
        }
    }
    if(i + 1 < M) {
        if(rabbit_cnt == 0 && world[i + 1][j].current_entity == EMPTY) {
            empty_neighbors[empty_cnt].i = i + 1;
            empty_neighbors[empty_cnt].j = j;
            empty_cnt++;
        }
        else if(world[i + 1][j].current_entity == RABBIT) {
            rabbit_neighbors[rabbit_cnt].i = i + 1;
            rabbit_neighbors[rabbit_cnt].j = j;
            rabbit_cnt++;
        }
    }
    if(j - 1 >= 0) {
        if(rabbit_cnt == 0 && world[i][j - 1].current_entity == EMPTY) {
            empty_neighbors[empty_cnt].i = i;
            empty_neighbors[empty_cnt].j = j - 1;
            empty_cnt++;
        }
        else if(world[i][j - 1].current_entity == RABBIT) {
            rabbit_neighbors[rabbit_cnt].i = i;
            rabbit_neighbors[rabbit_cnt].j = j - 1;
            rabbit_cnt++;
        }
    }

    if(rabbit_cnt > 0) {
        p = &rabbit_neighbors[cell_number % rabbit_cnt];
    }
    else if(empty_cnt > 0) {
        p = &empty_neighbors[cell_number % empty_cnt];
    }
    else{
        return;
    }

    if(world[i][j].breeding_age >= fbreed) {
        world[i][j].breeding_age = 0;
        new_fox = 1;
    }

    // Conflict resolution
    switch(world[p->i][p->j].entity) {
        case EMPTY:
            world[p->i][p->j].entity = world[i][j].entity;
            world[p->i][p->j].breeding_age = world[i][j].breeding_age;
            world[p->i][p->j].starving_age = world[i][j].starving_age;
            world[p->i][p->j].last_move_gen = gen;
            break;
        case RABBIT:
            world[p->i][p->j].entity = world[i][j].entity;
            world[p->i][p->j].breeding_age = world[i][j].breeding_age;
            world[p->i][p->j].starving_age = 0;
            world[p->i][p->j].last_move_gen = gen;
            break;
        case FOX:
            if(world[i][j].breeding_age > world[p->i][p->j].breeding_age) {
                world[p->i][p->j].breeding_age = world[i][j].breeding_age;
                if(world[p->i][p->j].starving_age != 0)
                    world[p->i][p->j].starving_age = world[i][j].starving_age;
            }
            else if(world[i][j].breeding_age == world[p->i][p->j].breeding_age) {
                if(world[i][j].starving_age < world[p->i][p->j].starving_age)
                    world[p->i][p->j].starving_age = world[i][j].starving_age;
            }
            break;
    }

    // Breeding or not
    if(new_fox) {
        world[i][j].starving_age = 0;
    }
    else {
        reset_position(i, j);
    }
}

void move(int i, int j, int gen) {

    if(world[i][j].last_move_gen == gen) return;

    switch(world[i][j].entity) {
        
        case RABBIT:
            move_rabbit(i, j, gen);
            break;
        case FOX:
            move_fox(i, j, gen);
            break;
    }
}

void run_simulation() {
    int i, j, k, gen, offset;
    
    int n_lines = 8;

    #pragma omp parallel private(i, j, k, gen, offset)
    for(gen = 1; gen <= ngenerations; gen++){

        // RED SUB-GENERATION

        #pragma omp single
        {

            k = n_lines;
            for(i = 0; i < M; i += 2 * n_lines) {

                if(i + n_lines > M) k = M - i;

                #pragma omp task depend(out: world[i:k][:N])
                for(offset = 0; offset < n_lines && i + offset < M; offset++) {
                    for(j = (i + offset) % 2; j < N; j += 2){
                        move((i + offset), j, gen);
                    }
                }
            }

            k = n_lines;
            for(i = n_lines; i < M; i += 2 * n_lines) {

                if(i + n_lines > M) {
                    #pragma omp task depend(in: world[i - n_lines:n_lines][:N])
                    for(offset = 0; offset < n_lines && i + offset < M; offset++) {
                        for(j = (i + offset) % 2; j < N; j += 2){
                            move((i + offset), j, gen);
                        }
                    }
                }
                else {

                    if(i + 2 * n_lines > M) k = M - i - n_lines;

                    #pragma omp task depend(in: world[i - n_lines:n_lines][:N], world[i + n_lines:k][:N])
                    for(offset = 0; offset < n_lines && i + offset < M; offset++) {
                        for(j = (i + offset) % 2; j < N; j += 2){
                            move((i + offset), j, gen);
                        }
                    }
                }
            }
        }

        #pragma omp for schedule(dynamic, n_lines)
        for(i = 0; i < M; i++){
            for(j = i % 2; j < N; j += 2){
                world[i][j].current_entity = world[i][j].entity;
            }
        }

        // BLACK SUB-GENERATION

        #pragma omp single
        {
            k = n_lines;
            for(i = 0; i < M; i += 2 * n_lines) {

                if(i + n_lines > M) k = M - i;

                #pragma omp task depend(out: world[i:k][:N])
                for(offset = 0; offset < n_lines && i + offset < M; offset++) {
                    for(j = (i + offset + 1) % 2; j < N; j += 2){
                        move((i + offset), j, gen);
                    }
                }
            }

            k = n_lines;
            for(i = n_lines; i < M; i += 2 * n_lines) {

                if(i + n_lines > M) {
                    #pragma omp task depend(in: world[i - n_lines:n_lines][:N])
                    for(offset = 0; offset < n_lines && i + offset < M; offset++) {
                        for(j = (i + offset + 1) % 2; j < N; j += 2){
                            move((i + offset), j, gen);
                        }
                    }
                }
                else {

                    if(i + 2 * n_lines > M) k = M - i - n_lines;

                    #pragma omp task depend(in: world[i - n_lines:n_lines][:N], world[i + n_lines:k][:N])
                    for(offset = 0; offset < n_lines && i + offset < M; offset++) {
                        for(j = (i + offset + 1) % 2; j < N; j += 2){
                            move((i + offset), j, gen);
                        }
                    }
                }
            }
        }

        #pragma omp for schedule(dynamic, n_lines)
        for(i = 0; i < M; i++){
            for(j = 0; j < N; j++){
                world[i][j].current_entity = world[i][j].entity;
                // Check for the death of the fox
                if(world[i][j].entity == FOX && world[i][j].starving_age >= fstarv) {
                    reset_position(i, j);
                }
            }
        }
    }
}

void print_result() {
    int i, j;
    nrab = nfox = 0;

    for(i = 0; i < M; i++){
        for(j = 0; j < N; j++){
            switch (world[i][j].current_entity)
            {
                case RABBIT:
                    nrab++;
                    break;
                case FOX:
                    nfox++;
                    break;
            }
        }
    }

    printf("%d %d %d\n", nrock, nrab, nfox);
}

int main(int argc, char *argv[]) {
    double exec_time;
    int i, j;

    ngenerations = atoi(argv[1]);
    M = atoi(argv[2]);
    N = atoi(argv[3]);
    nrock = atoi(argv[4]);
    nrab = atoi(argv[5]);
    rbreed = atoi(argv[6]);
    nfox = atoi(argv[7]);
    fbreed = atoi(argv[8]);
    fstarv = atoi(argv[9]);
    seed = atoi(argv[10]);

    world = (data **) malloc(M * sizeof(data *));
    for(i = 0; i < M; i++){
        world[i] = (data *) malloc(N * sizeof(data));
        for(j = 0; j < N; j++){
            reset_position(i, j);
        }
    }

    nrock = generate_element(nrock, ROCK, &seed);
    nrab = generate_element(nrab, RABBIT, &seed);
    nfox = generate_element(nfox, FOX, &seed);

    exec_time = -omp_get_wtime();

    run_simulation();

    exec_time += omp_get_wtime();

    fprintf(stderr, "%.1fs\n", exec_time);

    print_result();

    for(i = 0; i < M; i++){
        free(world[i]);
    }
    free(world);

    return 0;
}