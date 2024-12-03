#include "log_data_rw.h"



// Helper function to count lines in file
int count_lines(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) return -1;
    
    int lines = 0;
    char buffer[1024];
    
    // Skip header
    fgets(buffer, sizeof(buffer), file);
    
    // Count data lines
    while (fgets(buffer, sizeof(buffer), file) != NULL) {
        lines++;
    }
    
    fclose(file);
    return lines;
}

// Function to load data from CSV file
LogData* load_log_data(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        printf("Error opening file: %s\n", filename);
        return NULL;
    }
    
    int num_lines = count_lines(filename);
    if (num_lines <= 0) {
        printf("Error: Empty or invalid file\n");
        fclose(file);
        return NULL;
    }
    
    LogData* data = malloc(sizeof(LogData));
    data->length = num_lines;
    data->time_s = malloc(num_lines * sizeof(float));
    data->current_value = malloc(num_lines * sizeof(float));
    data->current_angle = malloc(num_lines * sizeof(float));
    data->beta = NULL;
    
    // Skip header and store it for verification
    char buffer[1024];
    if (fgets(buffer, sizeof(buffer), file) == NULL) {
        printf("Error reading header\n");
        free(data->time_s);
        free(data->current_value);
        free(data->current_angle);
        free(data);
        fclose(file);
        return NULL;
    }
    printf("Header: %s", buffer);  // Print header for verification
    
    // Read data line by line
    int i = 0;
    while (i < num_lines && fgets(buffer, sizeof(buffer), file)) {
        // Print first few lines for debugging
        if (i < 5) {
            printf("Line %d: %s", i+1, buffer);
        }
        
        if (sscanf(buffer, "%f,%f,%f", 
                   &data->time_s[i], 
                   &data->current_value[i],
                   &data->current_angle[i]) != 3) {
            printf("Error parsing line %d: %s", i+1, buffer);
            continue;
        }
        i++;
    }
    
    data->length = i;  // Update to actual number of lines read
    printf("Successfully read %d lines\n", i);
    
    fclose(file);
    return data;
}