#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <mpi.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb/stb_image_write.h"

int width, height, channels;
unsigned char *input, *global_output;


/**
 * @brief       Load an image from a file
 *
 * @param       input_size      The input size
 * @param       output          The assigned output
 * @param       output_size     The output size
 * @param       world_rank      The thread number
 * @param       channels        The channels of the image (3 for RGB, 4 for RGBA)
 * @param       gray_channels   The channels of the gray image (1 for grayscale, 2 for grayscale + alpha)
 *
 * @return      { void }
 */
void img2gray(int input_size, unsigned char *output, int output_size, int world_rank, int channels, int gray_channels) {
    MPI_Barrier(MPI_COMM_WORLD);

    int inp = input_size*(world_rank);
    int count = 0;

    // Apply the algorithm to each pixel of the image
    for(unsigned char *p = input+ inp , *pg = output ; p != input+ input_size*(world_rank+1); p += channels, pg += gray_channels) {
        *pg = (uint8_t)((*p + *(p + 1) + *(p + 2)) / 3.0);
        if(channels == 4) *(pg + 1) = *(p + 3);
    }

    // Use MPI_Barrier to make sure all processes have finished writing to the output buffer
    MPI_Barrier(MPI_COMM_WORLD);

    // Use MPI_Gather to gather all the images to the root process
    MPI_Gather(output, output_size, MPI_UNSIGNED_CHAR, global_output, output_size, MPI_UNSIGNED_CHAR, 0, MPI_COMM_WORLD);

    // Finish the program
    MPI_Finalize();
}

/**
 * @brief       Load an image from a file
 *
 * @param       input_path  The image to load
 *
 * @return      { void }
 */
void read_image(char *input_path) {
    input = (unsigned char *)stbi_load(input_path, &width, &height, &channels, 0);
    if(input == NULL) {
        perror("Error loading the image");
        exit(1);
    }
}

/**
 * @brief       Save the image result to a file
 *
 * @param       output_path     Path to the output file
 * @param       output_channels Number of channels in the output image
 *
 * @return      { void }
 */
void write_output(char *output_path, int output_channels) {
    char *dot = strrchr(output_path, '.');
    char *ext = dot + 1;

    if (strcmp(ext, "jpg") == 0 || strcmp(ext, "jpeg") == 0)
        stbi_write_jpg(output_path, width, height, output_channels, global_output, 100);
    else if (strcmp(ext, "png") == 0)
        stbi_write_png(output_path, width, height, output_channels, global_output, width * output_channels);
    else if (strcmp(ext, "bmp") == 0)
        stbi_write_bmp(output_path, width, height, output_channels, global_output);
    else {
        printf("Output type is not jpg, png or bmp, defaulting to output.jpg\n");
        stbi_write_jpg("output.jpg", width, height, output_channels, global_output, 100);
    }
}

/**
 * @brief      { Main function }
 *
 * @param      argc  The path to the image file
 * @param      argv  The output path for the converted image
 *
 * @return     { int }
 */
int main(int argc, char **argv) {

    // Check the number of arguments and their format
    if (argc != 3) {
        printf("Usage: %s <input_image> <output_image>\n", argv[0]);
        exit(1);
    }

    char *path = argv[1];
    char *grayPath = argv[2];
    int world_size, world_rank, gray_channels;

    // Read the image path
    read_image(path);

    // Start MPI
    MPI_Init(&argc, &argv);

    // Total of processes
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    // Thread number
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    MPI_Barrier(MPI_COMM_WORLD);

    // Number of channels
    gray_channels = channels == 4 ? 2 : 1;

    // Matrix sizes
    int input_size = width * height * channels / world_size;
    int output_size = width * height * gray_channels / world_size;


    // Output assignment
    unsigned char *output = (unsigned char *)malloc(output_size*sizeof(unsigned char));

    global_output = (unsigned char *)malloc(output_size*world_size*sizeof(unsigned char));

    // Error management
    if(output == NULL) {
        perror("Unable to allocate memory for the gray image");
        exit(1);
    }

    // Take the time for the algorithm execution and start the algorithm
    clock_t begin = clock();

    img2gray(input_size, output, output_size, world_rank, channels, gray_channels);

    clock_t end = clock();
    double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;

    // Check if the process is the root process and save the output
    if(world_rank == 0) write_output(grayPath, gray_channels);

    // Free the memory
    stbi_image_free(input);
    stbi_image_free(output);

    // Show the time spent
    printf("Execution time for rank %d: %f\n", world_rank, time_spent);

    return 0;
}
