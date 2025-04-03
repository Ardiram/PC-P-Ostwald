//  if you need any help with this code, please send an email to
// detlef.diesing@uni-due.de

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>





static uint64_t s[2] = {1234567890987654321ULL, 9876543210123456789ULL};



// Parameters easily adjustable by the user concerning the surface size

# define NUMBER_OF_COLUMNS 250  // surface size in j (horizontal) direction, the default value taken in the Journal of Chemical Education article is 250
// value must be larger than 10

# define NUMBER_OF_ROWS  250    // surface size in i (vertical) direction , the default value taken in the Journal of Chemical Education article is 250
// value must be larger than 10


//  Remark: The eps output files of the adsorbate structure are not optimized for NUMBER_OF_COLUMNS not equal to  NUMBER_OF_ROWS but you can play with it.

# define UPPER_LIMIT_FOR_SUBSTRATE_DISPLAY  90 // this is to limit the size of the eps output files

// if UPPER_LIMIT_FOR_SUBSTRATE_DISPLAY is larger than NUMBER_OF_COLUMNS, then all atoms  of two layers of the substrate (C , B) and the adatom layer are displayed.
// the reader should try it least one time to set UPPER_LIMIT_FOR_SUBSTRATE_DISPLAY > NUMBER_OF_COLUMNS to have the optical impression of the crystal layers of the 111 surface

# define NUMBER_OF_PREADSORBED_ADSORBATES 0

// The amount of adsorbates  "NUMBER_OF_PREADSORBED_ADSORBATES" is statistically distributed on the surface before starting the Monte Carlo process.

//  This is desirable for a faster start of the programm  on large matrices with number of colums and number of rows larger than 700 , for  a simulation of the growth process focussing on small coverages  this value should be set to 0


// # define DIAGNOSTICS_TOOLS false

 # define DIAGNOSTICS_TOOLS true

// if  DIAGNOSTICS_TOOLS  is set  true, the eps output files show each adsorbate tagged with the adsorbate id. An additional csv file is written which contains all adosrbates and their diffusion rates in all six directions, number of neighbours and so on.
// The font size in the eps output is optimized in this case  for NUMBER_OF_COLUMNS=NUMBER_OF_ROWS=200


// Following constants are used


# define PREEXPFACTOR_DIFFUSION 1E11

# define ELECTRON_CHARGE 1.6019e-19

# define K_BOLTZMANN 1.3807e-23


// The following values for the energies are used, all in electron volt
// These values can be chosen by the student

# define DIFFUSION_ACTIVATION_ENERGY 0.1

# define NN_ENERGY 0.025  //   positive values mean attractive interaction
// NN stands for nearest neighbour (first order)

# define NNN_ENERGY  0.005 // NNN stands for next nearest neighbour (second order)

# define NNNN_ENERGY 0.00125 // NNNN stands for next next nearest neighbour (third order)

// see figure 2 in the manuscript for the defintion of neighbours


# define BARRIER_DEFORMATION_FACTOR 1.35

// see figure 3 in the manuscript


# define VIA_ON_TOP_DIFFUSION_FACTOR 1.375

// This factor increases the barrier or activation energy via on top diffusion
// see figure 4  in the manuscript


// The following parameters have to be chosen in accordance with the chemical experiment
// These can be adjusted by the student


# define DEPOSITION_RATE  5e-3   //  in monolayers per second

# define FINAL_COVERAGE 0.01   // this one was changed september 2023 from 0.02

# define TEMPERATURE_DURING_GROWTH  60  // all temperatures in Kelvin

# define TEMPERATURE_DURING_RIPENING  210

# define DURATION_OF_HEATING_PROCESS 10     //  time in seconds needed to heat the sample from TEMPERATURE_DURING_GROWTH to TEMPERATURE_DURING_RIPENING
// for the set default values this corresponds to a heating rate of 12 Kelvin per second. This is achievable with modern laboratory equipment.

# define DURATION_OF_RIPENING_AT_CONSTANT_TEMPERATURE  60 //  this value should be set to values between 10 sec and 60 sec

// 10 sec is a reasonable minimum value, so you observe ripening and agglomeration at constant temperature at least as long as ripening and agglomeration (achieved with the default values) during the heating process
// 60 sec as maximum value will ensure that artifacts caused by the periodic boundary conditions do not influence your adsorbate structure

# define FRAME_RATE 5 // this means for the set value that 5 times per  second (every 0.2  s sec) in monte carlo time there will be an eps or svg  file of the adsorbate structure and a new line in the experiment monitor output.

# define DURATION_FACTOR_ANIMATED_SVG 10

# define STROKE_WIDTH 0.1

# define DATA_DIRECTORY ""






// The DATA_DIRECTORY in line 111 is the path where your calculated data will be

// chose "" in line 111 if you expect your data to be in the same folder as your excecutable is. This ususally works well for Windows operating systems. Then your data will be in the same folder as your growth_ripen.exe file. Simply double click on the growth_ripen.exe file in the Windows Explorer to start the programm




// The following five integer values determine the  data structure
// These values should only be changed when the code is rewrittten, for example when the code is applied to a  100 face.


# define NUMBER_OF_COLUMNS_IN_TABLE_OF_ADSORBATE_DATA 9  // there are the following 9 values in one line of the table of adsorbate data
// adsorbate_id, i_coordinate of the adsorbate,j_coordinate, number of first order neighbours, number of second order neighbours, number of third order neighbours, diffusion direction, diffusion rate, process_id
// This means that for every adsorbate there are 6 lines in this table, the first six columns of this line being identical for all of them.

// one comment on the process id: this might be used for later application. It is calculated as follows:
// the process id for adsorbate 1 for diffusion in direction 1 is 1 , in direction 6  the process id is 6
// for adsorbate 2 the process ids are 7 to 12 for the diffusion directions 1 to 6



# define NUMBER_OF_COLUMNS_IN_EXPERIMENT_MONITOR_FILE 15  // in one line of this file there are the following columns
// mc_time, temperature, number_adsorbates, coverage, mc_step, cpu_time, rewrites, sorting, diff1, diff2, diff3, diff4, diff5, diff6, percentage of atoms with six neighbours

# define NUMBER_OF_COLUMNS_IN_HISTORY_OF_ADSORBATE_POSITIONS  6 // first column the index starting with 1 and going to mcs.number_of_eps_files_and_lines_in_experiment_monitor_file,   index = 1 would point to monte carlo time = 50 ms if the FRAME_RATE is set to 20, index =  2 would point to t = 0.1 s and should show the snapshot of the adsorbate positions at this time
// the actual values of the value in the first column is mcs.line_in_experiment_monitor_file
// the second column is the total number of adsorbates at this time.
// the third column is the ad_id going from 1 to total number of adsorbates
// the fourth column is the i_coordinate of this adsorbate at this index (time)
// the fifth column is the j_coordinate of this adsorbate at this index (time)
// the sixth column is the temperature at this index (time)


# define NUMBER_OF_INTEGER_VALUES_IN_PROCESS_COUNTER 9
// number of diffusion processes direction 1, 2, 3, 4, 5, 6, adsorption, rewrites of single rates , sorting events

# define NUMBER_OF_COLUMNS_IN_TABLE_OF_ADSORBATES_INFLUENCED_BY_DIFFUSION 1  // in one line
//  adsorbate_id


# define MAXIMUM_NUMBER_OF_ADSORBATES_INFLUENCED_BY_DIFFUSION 36  //
// the maximum number of adsorbates which can be influenced by a diffusion process
// It is 3 times 6 neighbours for the initial position, the same for the final position, and the final position itself  (3*6*2=36)


# define FONT_SIZE_LEGEND 6  // in pt

# define CIRCLE_RADIUS_LEGEND 2  //  taken in mm , so use roughly 33 percent of the FONT_SIZE_LEGEND in pt
// or more precisely 25.4/72






struct MCS_STRUCT{
    clock_t start_clock_of_mcs,clock_actual_output,clock_last_output;
    char *fname_experiment_monitor;
    char *fname_process_counter;
    int number_of_adsorbates;
    int number_of_expected_adsorbates;
    int maximum_number_of_adsorbates;
    int number_of_adsorbates_with_less_than_six_neigh;
    int number_of_adsorbates_influenced_by_diffusion;
    int rewrite_counter; // increased by 1 when adsorption happened or when a diffusion event influences the number of neighbours of other adsorbates
    double **table_of_adsorbate_data; // length of this is number_of_adsorbates
    int **table_of_adsorbates_influenced_by_diffusion; // contains the adsorbate ID
    int **history_of_adsorbate_positions; // only necessary for animated svg output
    int lines_in_history_of_adsorbates_positions;
    double **swap_line_for_table_of_adsorbate_data;
    double **experiment_monitor;
    int diff_possibilities_evaluated_in_one_mc_step;
    int *process_counter; // elements 1 to 6 account for the 6 diffusion directions, element 7 for the adsorption process
    double *list_mc_times_monitored;
    double sum_of_diffusion_rates;
    double sum_of_adsorption_rates;
    int lines_in_experiment_monitor_file;
    int number_of_eps_files_and_lines_in_experiment_monitor_file;
    int line_in_experiment_monitor_file;
    int id_of_moving_adsorbate;
    int id_of_touched_adsorbate;
    int diffusion_direction_selected;   // from 1 to 6 on a 111 surface
    double random_number; //random number values chosen by procedure mc_step between 0 and 1
    double mc_time;
    long int mc_step_actual_output, mc_step_last_output,number_mc_step;
    double time_increment; // delta t for the actual monte carlo step
    double cpu_time; // consumed time since program start
    double time_per_step_per_adsorbate;
    double temperature;
    double deposition_rate;
    double mean_time_between_two_adsorption_events;
    double duration_of_growth;
    double duration_of_experiment;
    double diagnostics_delta_time_deposition_step;
    double single_adsorbate_diffusion_time;
    bool   write_adsorbate_matrix,rewrite_data_for_selected_atoms_necessary,growth_finished,sorting_and_counting_necessary,diagnostics_tools;
    
    
} mcs;





int i_periodic(int i, struct MCS_STRUCT *ptr_to_mcs_struct)
{  // this procecdure ensures the periodic boundary conditions in i direction
    
    ptr_to_mcs_struct=&mcs;
    
    int output=0;
    
    
    if (i>=1 && i<=NUMBER_OF_ROWS) {
        output=i;
        
    }
    
    
    
    
    //  periodic boundary conditions
    
    
    if (i==0) {
        output=NUMBER_OF_ROWS;
    }
    
    if (i==-1) {
        output=NUMBER_OF_ROWS-1;
    }
    
    if (i==-2) {
        output=NUMBER_OF_ROWS-2;
    }
    
    
    
    
    
    if (i==NUMBER_OF_ROWS+1) {
        output=1;
    }
    
    
    if (i==NUMBER_OF_ROWS+2) {
        output=2;
    }
    
    if (i==NUMBER_OF_ROWS+3) {
        output=3;
    }
    
    
    
    
    
    
    
    return output;
    
}


int j_periodic(int j, struct  MCS_STRUCT *ptr_to_mcs_struct)
{    // this procecdure ensures the periodic boundary conditions in j direction
    
    ptr_to_mcs_struct =&mcs;
    int output=0;
    
    if (j>=1 && j<=NUMBER_OF_COLUMNS) {
        output=j;
        
    }
    
    
    if (j==0) {
        output=NUMBER_OF_COLUMNS;
    }
    
    if (j==-1) {
        output=NUMBER_OF_COLUMNS-1;
    }
    
    if (j==-2) {
        output=NUMBER_OF_COLUMNS-2;
    }
    
    
    
    if (j==NUMBER_OF_COLUMNS+1) {
        output=1;
    }
    
    if (j==NUMBER_OF_COLUMNS+2) {
        output=2;
    }
    
    if (j==NUMBER_OF_COLUMNS+3) {
        output=3;
    }
    
    
    
    
    
    return output;
    
}




double random_number(void)
{ // this procecdure gives a randonum number between zero and one
    double return_value;
    double maximum;
    return_value=(double )rand();
    maximum=(double)RAND_MAX;
    return_value=return_value/maximum;
    return return_value;
    
}

/*
 
 int random_integer(int int_max)
 {  //this procedure gives random numbers between 1 and int_max dies Prozedur ist handgemacht
 //it is used in the procedure perform_adsorption to roll the dice for setting an adsorbate to random coordinates
 
 
 int output_value;
 
 output_value=1+(rand() % int_max);
 
 return output_value;
 
 
 }
 
 */

int random_integer(int int_max)
{
    return rand() % int_max + 1;
}



/*
 
 int random_integer(int int_max)
 {  // this is bitwise from Claude AI, int_max needs to be o power of 2 minus 1
 return (rand() & int_max) + 1;
 }
 
 
 */



int free_occupied_detector(int i_selected, int j_selected,struct MCS_STRUCT *ptr_to_mcs_struct)
{
    // this procedure gives the return value 1 if there is an adsorbate on the coordinates (i_selected,j_selected).
    // this procedure give 0 if there is no adsorbate on these coordinates.
    
    ptr_to_mcs_struct=&mcs;
    
    int return_value=0;
    int i=0;
    // i is the index in table of adsorbate data
    
    // check whether the selected coordinates are outside the matrix.
    // if yes the return_value will be 0s and 1s  weighted with the occupation as probability
    
    
    
    
    
    for (i=1; i<=mcs.number_of_adsorbates*6; i++) {
        
        
        if ((int)(mcs.table_of_adsorbate_data[i][2])==i_selected &&(int)(mcs.table_of_adsorbate_data[i][3])==j_selected) {
            
            mcs.id_of_touched_adsorbate=(int)(mcs.table_of_adsorbate_data[i][1]);
            
            return_value=1;
            
            break;
        }
        
        
        
    }
    
    
    return return_value;
    
}



int compare_neighbours_in_table_of_adsorbate_data (const void *pa, const void *pb )
{ // this procedure is used by qsort , it allows a sorting in asending order.  Adsorbates with high numbers of neighbours in first order will appear at the end of the table
    // this means that trapped atoms with six nearest neighbours will be at the end of the table
    const double *a = *(const double **) pa;
    const double *b = *(const double **) pb;
    
    if ( a[4]  <  b[4] ) {return -1;}
    if ( a[4]  >  b[4] ) {return +1;}
    
    return 0;
    
    // this is a sorting in asending order adsorbates with high numbers of neighbours in first order will appear at the end of the table
    
    
    
}



int compare_rates_in_table_of_adsorbate_data (const void *pa, const void *pb )
{
    // This procedure is not used by the program , it might be of interest for people changing the code
    
    // this is a sorting in descending order -- high rates will be on  top of the table
    
    
    const double *a = *(const double **) pa;
    const double *b = *(const double **) pb;
    // int output_value;
    
    if (a[8] > b[8] ) {return -1;}
    if (a[8] < b[8] ) {return +1;}
    
    return 0;
    
    
}




void print_adsorbate_data(struct MCS_STRUCT *ptr_to_mcs_struct){
    
    // this is a diagnostic routine, it prints the data of the adsorbate table  if an error occurrs.
    
    
    ptr_to_mcs_struct=&mcs;
    
    int i,j;
    
    printf("I print on demand the actual adsorbate structure\n");
    
    printf("ad_id \t\ti  \t\tj  \t\tn1  \tn2  \tn3  \n" );
    
    
    
    for (i=1; i<=mcs.number_of_adsorbates*6 ; i++) {
        
        for (j=1; j<=6; j++) {
            
            if (j==1 ) {
                printf("%3d\t\t\t",(int) (mcs.table_of_adsorbate_data[i][j] ) );
                
            }
            
            
            if (j==2 || j==3) {
                printf("%3d  \t",(int) (mcs.table_of_adsorbate_data[i][j] ) );
                
            }
            
            if (j>=4) {
                printf("%1d\t\t",(int) (mcs.table_of_adsorbate_data[i][j] ) );
                
            }
            
            
            
        }
        
        printf("\n");
        
        
        
    }
    
    
    
    
    
    printf("now print the rates \n\n");
    
    printf("ad_id \tdiffdir \t rate \t\t process_id\n" );
    
    
    for (i=1; i<=mcs.number_of_adsorbates*6 ; i++) {
        
        
        printf("%3d  \t ",(int) (mcs.table_of_adsorbate_data[i][1])  );
        
        printf("  %1d  \t ",(int) (mcs.table_of_adsorbate_data[i][7])  );
        
        printf("%05.2g\t", (mcs.table_of_adsorbate_data[i][8]) );
        
        printf("\t%3d\n", ((int)(mcs.table_of_adsorbate_data[i][9])) );
        
        
        
    }
    
    
    // resort so that atoms with  high number of neighbours are below
    
    qsort(mcs.table_of_adsorbate_data+1,mcs.number_of_adsorbates*6,sizeof(mcs.table_of_adsorbate_data[0]),compare_neighbours_in_table_of_adsorbate_data);
    
    printf("\n\n\n\n\n");
    
    
    
    
    
}



void print_history_of_adsorbate_data(struct MCS_STRUCT *ptr_to_mcs_struct){
    
    // this is a diagnostic routine, it prints the history of adsorbate data on demand.
    
    
    ptr_to_mcs_struct=&mcs;
    
    int i,j;
    
    int old_line_in_experiment_monitor_file = 0;
    
    int new_line_in_experiment_monitor_file = 0;
    
    
    
    printf("this is the history output , the actual number of adsorbates is %i \n\n\n",mcs.number_of_adsorbates);
    
    printf("index \t\tnum_adsor \t\t\tad_id    \ti_coor  \tj_coor  \tTemperature  \n" );
    
    
    
    for (i=1; i<=mcs.lines_in_history_of_adsorbates_positions ; i++) {
        
        old_line_in_experiment_monitor_file=mcs.history_of_adsorbate_positions[i-1][1];
        
        new_line_in_experiment_monitor_file=mcs.history_of_adsorbate_positions[i][1];
        
        if (new_line_in_experiment_monitor_file> old_line_in_experiment_monitor_file ) {
            printf("-----------------------------------------------\n");
            
            
        }
        
        
        
        for (j=1; j<=6; j++) {
            
            if (j==1) {
                printf("%4d\t\t",(int) (mcs.history_of_adsorbate_positions[i][j] ) );
                // fprintf(stdout, "%4d\t\t",(int) (mcs.history_of_adsorbate_positions[i][j] )    );
                
            }
            
            if (j>=2) {
                printf("\t\t%3d\t",(int) (mcs.history_of_adsorbate_positions[i][j] ) );
                
                //   fprintf(stdout, "%4d\t\t",(int) (mcs.history_of_adsorbate_positions[i][j] )    );
                
                
                
            }
            
            
        }
        
        
        printf("\n");
        
        
        // check whether there is a new line in experiment monitor file
        
        
        
        
        
        
    }
    
    
    
    
}




void print_influenced_atoms(struct MCS_STRUCT *ptr_to_mcs_struct){
    
    // this is a diagnostic routine, it prints the data of the atoms whose number of neighbours were changed by the last diffusion step
    
    ptr_to_mcs_struct=&mcs;
    
    int i;
    
    printf("i<=37 \tad_id \n" );
    
    
    for (i=1; i<=mcs.number_of_adsorbates_influenced_by_diffusion; i++) {
        
        printf("%2d     \t %3d \n",i,mcs.table_of_adsorbates_influenced_by_diffusion[i][1]);
        
    }
    
}


void check_sum_of_diffusion_rates(struct MCS_STRUCT *ptr_to_mcs_struct)
{
    // this is a diagnostic routine. It is called at monte carlo step 10 and every half a million monte carlo steps later
    // this routine calculates the sum of diffusion rates
    
    // This value is compared with the value stored in mcs.sum_of_diffusion_rates.
    
    //The value mcs.sum_of_diffusion_rates is calculated again and again in every Monte Carlo step by adding only the changes
    
    // This procedure is a kind of rounding error watch dog
    
    
    ptr_to_mcs_struct=&mcs;
    
    double sum_of_diffusion_rates=0.0;
    int i;
    
    for (i=1; i<=mcs.number_of_adsorbates*6; i++) {
        
        sum_of_diffusion_rates+=mcs.table_of_adsorbate_data[i][8];
        
    }
    
    
    
    // prepare error message
    
    if (fabs( (mcs.sum_of_diffusion_rates-sum_of_diffusion_rates)/sum_of_diffusion_rates  ) > 0.1 ){
        
        printf("\n\n\nAttenion Error message !!! \n\n\nA rounding error of at least 10 %% occurred in the determination of the sum of diffusion rates\nIt has been corrected now at mc time %g sec at mc step number %ld \n",mcs.mc_time, mcs.number_mc_step);
        
    }
    
    else{
        
        
        printf("\n\nI compare here  at monte carlo step number %ld the newly calculated value for the sum of diffusion rates \n",mcs.number_mc_step);
        
        printf("%g\n", sum_of_diffusion_rates);
        
        printf("with the one calculated again and again by adding only the changes during the last 500.000 MC steps \n");
        
        printf("%g\n", mcs.sum_of_diffusion_rates);
        
        printf("Everything seems to be ok\n");
        
        
        
        
        
        
    }
    
    
    
    
    mcs.sum_of_diffusion_rates=sum_of_diffusion_rates;
    
    
}



// void data_output_csv(struct MCS_STRUCT *ptr_to_mcs_struct){
//     
//     // this is a diagnostic routine, it prints the adsorbate table to a csv file if the DIAGNOSTICS_TOOLS in line 34 of are set to true
//     
//     // this routine writes a csv files with the adsorbate id, the adsorbate coordinates, the number of nearest neighbours next nearest neigbhours n2, next next nearest neighbours n3 and so on.
//     ptr_to_mcs_struct=&mcs;
//     
//     FILE *fhd1;
//     
//     int i,j;
//     
//     char first_part_of_local_filename[200]="";
//     
//     char second_part_of_local_filename[200]="";
//     
//     char local_filename[200]="";
//     
//     strcat(first_part_of_local_filename,DATA_DIRECTORY);
//     
//     sprintf(second_part_of_local_filename,"adsorbate_table_for_T_growth_%03d_K_for_mc_step_%09ld.csv",TEMPERATURE_DURING_GROWTH,mcs.number_mc_step);
//     
//     strcat(local_filename,first_part_of_local_filename);
//     
//     strcat(local_filename,second_part_of_local_filename);
//     
//     fhd1=fopen(local_filename,"w+");
//     
//     
//     fprintf(fhd1, "adsorbate_id, i_coor, j_coor, n1, n2, n3, diffdir, diffrate, pid  \n");
//     
//     for (i=1; i<=mcs.number_of_adsorbates*6; i++) {
//         
//         for (j=1; j<=NUMBER_OF_COLUMNS_IN_TABLE_OF_ADSORBATE_DATA; j++) {
//             
//             if (j>=1 && j<=7 ) {
//                 fprintf(fhd1, "%03d,  ",(int) (mcs.table_of_adsorbate_data[i][j]) );
//                 
//             }
//             
//             
//             if (j==8 ) {
//                 fprintf(fhd1, "%g,  ",mcs.table_of_adsorbate_data[i][j] );
//                 
//             }
//             
//             if (j==9 ) {
//                 fprintf(fhd1, "%04d\n  ",(int) (mcs.table_of_adsorbate_data[i][j] ));
//                 
//             }
//             
//             
//             
//         }
//         
//         
//         
//         
//     }
//     
//     
//     fclose(fhd1);
//     
//     
//     
// }




void data_output_svg(struct MCS_STRUCT *ptr_to_mcs_struct){
    
    // this procedure writes a file of the adsorbate structure in svg format
    
    ptr_to_mcs_struct=&mcs;
    
    FILE *fhd1;
    
    int i,j;
    
    int temperature_growth=TEMPERATURE_DURING_GROWTH;
    
    char local_filename[200]="";
    
    char number[200];
    
    double x_coordinate=0.0,y_coordinate=0.0,delta_x=0.0,delta_y=0.0;
    
    int font_size_legend=9;
    
    double line_spacing=1.5*font_size_legend;
    
    //  a double column figure in a journal typically has a width of 16 cm
    //  this means 16/2.54 inches
    //  Postscript default resolution 72 points pro inch
    
    double final_page_width=16./2.54*72;;
    
    double tentative_page_height=final_page_width/(sqrt(2.));
    
    double relative_sphere_radius=1.0;
    
    double sphere_radius=tentative_page_height/(2.*relative_sphere_radius+NUMBER_OF_ROWS*sqrt(3)*relative_sphere_radius);
    
    sphere_radius = floor(sphere_radius*10)/10;
    
    //double y_value_lowest_row=font_size_legend*1;
    
    double test_bottom_right_y=sphere_radius*(2.-sqrt(3)/3.+sqrt(3)*NUMBER_OF_ROWS);
    
    double test_bottom_right_x=4*sphere_radius+(NUMBER_OF_COLUMNS+NUMBER_OF_ROWS/2.-3./2.)*sphere_radius*2;
    
    double measured_page_height=test_bottom_right_y;
    
    double measured_page_width=test_bottom_right_x;
    
    double correctur_factor=measured_page_width/measured_page_height;
    
    double page_width=final_page_width;
    
    double page_height=page_width/correctur_factor;
    
    
    // recalculation of the sphere radius
    
    
    sphere_radius=page_height/(2.*relative_sphere_radius+NUMBER_OF_ROWS*sqrt(3)*relative_sphere_radius);
    
    sphere_radius = floor(sphere_radius*10)/10;
    
    
    // Layer C is shifted towards layer B //
    
    // The shift in x-direction is called delta_x , its value is sphere_radius
    // The shift in x-direction is called delta_y , its value is minus sphere_radius times sqrt(3)/3
    // the later comes from the geometrical calculation that the layer C atom is just in the middle of
    // an equilateral triangle which is spanned by three atoms of the layer B (the layer below C)
    
    delta_x=sphere_radius;
    
    delta_y=sphere_radius*tan(M_PI/6);
    
    
    
    // in case of number of columns and rows larger than for example 150 do not write the layers B and C but a simple gray parallelogramm instead
    
    // define the parallelogramm which replaces layer c for greater matrices
    
    
    // seen from the top left corner of the page
    
    double
    
    top_left_corner_parallelogram_x=2*sphere_radius ,
    
    top_left_corner_parallelogram_y=-1.*sphere_radius*(1.+sqrt(3)/3.),
    
    bottom_left_corner_parallelogram_x=sphere_radius+sphere_radius*NUMBER_OF_ROWS,
    
    bottom_left_corner_parallelogram_y=(.1547005390*sphere_radius-1.732050808*sphere_radius*NUMBER_OF_ROWS),
    
    top_right_corner_parallelogram_x=2*sphere_radius*NUMBER_OF_COLUMNS,
    
    top_right_corner_parallelogram_y= top_left_corner_parallelogram_y, // ist ungeaendert
    
    bottom_right_corner_parallelogram_x=-1.*sphere_radius+2.*sphere_radius*NUMBER_OF_COLUMNS+sphere_radius*NUMBER_OF_ROWS,
    
    bottom_right_corner_parallelogram_y=(.1547005390*sphere_radius-1.732050808*sphere_radius*NUMBER_OF_ROWS);
    
    
    
    // The layers B, C and the adsorbate layer are shifted vs each other
    // by sphere_radius in x and by sphere_radius*tan(30 deg) in y
    
    
    int seconds_experiment=0,milli_seconds_experiment=0;
    
    
    if (mcs.mc_time<1) {
        
        milli_seconds_experiment=(int) (trunc(mcs.mc_time*1000)) ;
        
        
        sprintf(number,"steps_%09ld_T_initial_%03iK_T_actual_%03iK_time_%03ims_%04i_adsorbates",mcs.number_mc_step,temperature_growth,(int)(mcs.temperature),milli_seconds_experiment,mcs.number_of_adsorbates);
        
        
        
    }
    
    
    if (mcs.mc_time>1 && mcs.mc_time<=mcs.duration_of_experiment) {
        
        
        seconds_experiment=(int) (trunc(mcs.mc_time)) ;
        
        milli_seconds_experiment=(int) (trunc(mcs.mc_time*1000-seconds_experiment*1000)) ;
        
        sprintf(number,"steps_%09ld_T_initial_%03iK_T_actual_%03iK_time_%02is_%03ims_%04i_adsorbates",mcs.number_mc_step,temperature_growth,(int)(mcs.temperature),seconds_experiment,milli_seconds_experiment,mcs.number_of_adsorbates);
        
        
        
    }
    
    char number_new[20];
    
    sprintf(number_new,"m");
    
    char line_in_experiment_monitor_file[1000];
    
    sprintf(line_in_experiment_monitor_file,"%03d",mcs.line_in_experiment_monitor_file);
    
    strcat(number_new,line_in_experiment_monitor_file);
    
    
    strcat(local_filename,DATA_DIRECTORY);
    
    //strcat(local_filename,"adsorb_struc_");
    
    strcat(local_filename,number_new);
    
    strcat(local_filename,".svg");
    
    
    fhd1=fopen(local_filename,"w+");
    
    
    if (fhd1==NULL) {
        
        printf("\n\n Warning \n\n The file named \n -->%s<-- \ncould not be opened due to an error\nPlease check the line 100 of the  source code\n",local_filename);
        
        
    }
    
    
    else {
        
        printf("\n\nThe file with the name \n-->%s<--\nwas successfully opened \n",local_filename);
        
        
    }
    
    // some helpers for svg
    
    // The following lines are taken from w3c.org
    
    char svg_header_line_one[91]="<?xml version=\"1.0\" standalone=\"no\"?>";
    
    char svg_header_line_two[]="<!DOCTYPE svg PUBLIC \"-//W3C//DTD SVG 1.1//EN\" \"http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd\">";
    
    // line three contains the page size and is defined later
    
    char svg_header_line_three[200]="";
    
    char svg_header_line_four[]="xmlns=\"http://www.w3.org/2000/svg\"";
    
    char svg_header_line_five[]="xmlns:xlink=\"http://www.w3.org/1999/xlink\">";
    
    // you find a help for this here: https://stackoverflow.com/questions/29884930/svg-namespace-prefix-xlink-for-href-on-textpath-is-not-defined
    
    //  define the header line three
    
    //    sprintf(svg_header_line_three,"<svg  viewBox=\"%g %g %g %g\" width=\"%gmm\" height=\"%gmm\" version=\"1.1\" ",(X_OFFSET_PERCENT/100.*imag_par.page_width),(Y_OFFSET_PERCENT/100.*imag_par.page_height) ,(imag_par.page_width/MAGNIFICATION_IN_VIEW_BOX),(imag_par.page_height/MAGNIFICATION_IN_VIEW_BOX),imag_par.page_width,imag_par.page_height);
    
    
    sprintf(svg_header_line_three,"<svg  viewBox=\"0 0 %i %i\" width=\"%gmm\" height=\"%gmm\" version=\"1.1\" ",(int)(ceil(page_width)),(int)(ceil(page_height)),page_width,page_height);
    
    
    
    fprintf(stdout, "%s\n",local_filename);
    
    
    
    if (fhd1==NULL) {
        
        printf("Error in opening the file with name\n -%s- ",local_filename);
        
        printf("\n\nplease check line number 47 in the programm code \n may be add or remove a / ? \n\n ");
        
        
    }
    
    else {
        
        
        printf("\n\n A  svg file  named \n -->%s<-- \nwas successfully written \n ",local_filename);
        
        
    }
    
    
    
    fprintf(fhd1,"%s\n",svg_header_line_one);
    
    fprintf(fhd1,"%s\n",svg_header_line_two);
    
    fprintf(fhd1,"%s\n",svg_header_line_three);
    
    fprintf(fhd1,"%s\n",svg_header_line_four);
    
    fprintf(fhd1,"%s\n",svg_header_line_five);
    
    
    
    // write the substrate layer B with circles everywhere
    
    if(NUMBER_OF_COLUMNS <= UPPER_LIMIT_FOR_SUBSTRATE_DISPLAY) {
        
        // write the substrate layer B with circles everywhere
        
        fprintf(fhd1,"\n<!--write the substrate layer B with circles everywhere -->\n");
        
        
        
        fprintf(fhd1, "\n<defs>");
        
        fprintf(fhd1, "\n<g id=\"B layer circle\"  style =\"stroke:rgb(64,64,64); stroke-width:%g; fill:rgb(64,64,64);stroke-opacity:1.0;fill-opacity:1.0;\" > ",STROKE_WIDTH);
        
        fprintf(fhd1,"\n<circle cx=\"%g\" cy=\"%g\" r=\"%g\" /> ",0.0,0.0,sphere_radius);
        
        fprintf(fhd1,"\n</g>");
        
        fprintf(fhd1, "\n</defs>");
        
        fprintf(fhd1,"\n");
        
        fprintf(fhd1,"\n");
        
        fprintf(fhd1, "\n<defs>");
        
        fprintf(fhd1, "\n<g id=\"substrate layer B with circles everywhere\">");
        
        
        
        
        for (i=1; i<=NUMBER_OF_ROWS; i++) {
            
            for (j=1; j<=NUMBER_OF_COLUMNS; j++){
                
                x_coordinate=(double)(sphere_radius+((j-1)+(i-1)/2.0)*(sphere_radius*2));
                // the upper line is the shift of each row with index i >1
                
                
                y_coordinate=(double)((sphere_radius+1*((i-1)*sphere_radius*sqrt(3))));
                
                
                y_coordinate = floor(y_coordinate*10)/10;
                
                
                
                fprintf(fhd1, "\n<use xlink:href=\"#B layer circle\" x=\"%g\" y=\"%g\"  />",x_coordinate,y_coordinate);
                
                
                
            }
            
        }
        
        fprintf(fhd1,"\n</g>");
        
        fprintf(fhd1, "\n</defs>");
        
        fprintf(fhd1,"\n");
        
        fprintf(fhd1,"\n");
        
        fprintf(fhd1,"\n");
        
        
        
        fprintf(fhd1, "\n<use xlink:href=\"#substrate layer B with circles everywhere\" transform=\"translate(%g,%g)\" />",sphere_radius,0.0);
        
        
        fprintf(fhd1,"\n");
        
        fprintf(fhd1,"\n");
        
        fprintf(fhd1,"\n");
        
        
        
        
        
    }
    
    
    
    
    // write the substrate layer B with outside circles and a parallelogramm inside
    
    
    if(NUMBER_OF_COLUMNS > UPPER_LIMIT_FOR_SUBSTRATE_DISPLAY) {
        
        
        
        // write the substrate layer B
        
        fprintf(fhd1,"\n");
        
        fprintf(fhd1,"\n");
      
        
        fprintf(fhd1,"\n<defs>");
        
        fprintf(fhd1, "\n<g id=\"B layer circle\"  style =\"stroke:rgb(64,64,64); stroke-width:%g; fill:rgb(64,64,64);stroke-opacity:1.0;fill-opacity:1.0;\" > \" ",STROKE_WIDTH);
        
        fprintf(fhd1,"\n<circle cx=\"%g\" cy=\"%g\" r=\"%g\" />",0.0,0.0,sphere_radius);
        
        fprintf(fhd1,"\n</g>");
        
        fprintf(fhd1, "\n</defs>");
        
        
        fprintf(fhd1,"\n");
        
        fprintf(fhd1,"\n");
        
        
        fprintf(fhd1, "\n<defs> ");
        
        fprintf(fhd1, "\n<g id=\"substrate layer B with outside circles\" >");
        
        
        
        fprintf(fhd1,"\n<!-- Hier mal schauen  a and b  only -->\n");
        
        
        
        
        
        for (i=1; i<=NUMBER_OF_ROWS; i++) {
            
            for (j=1; j<=NUMBER_OF_COLUMNS; j++){
                
                x_coordinate=(double)(sphere_radius+((j-1)+(i-1)/2.0)*(sphere_radius*2));
                // the upper line is the shift of each row with index i >1
                
                x_coordinate = floor(x_coordinate*10)/10;
                
                
                y_coordinate=(double)((sphere_radius+1*((i-1)*sphere_radius*sqrt(3))));
                
                y_coordinate = floor(y_coordinate*10)/10;
                
                
                if (i==1 || i == NUMBER_OF_ROWS || j==1 || j == NUMBER_OF_COLUMNS) {
                    
                    
                    fprintf(fhd1, "<use xlink:href=\"#B layer circle\" x=\"%g\" y=\"%g\"  />\n",x_coordinate,y_coordinate);
                    
                }
                
                
                
                if(i==1 && j==1){
                    
                    top_left_corner_parallelogram_x = x_coordinate;
                    top_left_corner_parallelogram_y = y_coordinate;
                    
                }
                
                
                if(i==1 && j==NUMBER_OF_COLUMNS){
                    
                    top_right_corner_parallelogram_x = x_coordinate;
                    top_right_corner_parallelogram_y = y_coordinate;
                    
                    
                }
                
                if(i==NUMBER_OF_ROWS && j==1){
                    
                    bottom_left_corner_parallelogram_x = x_coordinate;
                    bottom_left_corner_parallelogram_y = y_coordinate;
                    
                }
                
                
                if(i==NUMBER_OF_ROWS && j==NUMBER_OF_COLUMNS){
                    
                    bottom_right_corner_parallelogram_x = x_coordinate;
                    bottom_right_corner_parallelogram_y = y_coordinate;
                    
                }
                
                
                
                
                
                
            }
            
        }
        
        
        fprintf(fhd1, "\n<!-- the parallelogramm for layer B -->\n");
        
        fprintf(fhd1, "\n<polyline points= \"%g %g, %g %g,%g %g,%g %g \" style = \"stroke:rgb(64,64,64); stroke-width:%g; fill:rgb(64,64,64);stroke-opacity:1.0;fill-opacity:1.0;\">",top_left_corner_parallelogram_x,top_left_corner_parallelogram_y,bottom_left_corner_parallelogram_x,bottom_left_corner_parallelogram_y,bottom_right_corner_parallelogram_x,bottom_right_corner_parallelogram_y,top_right_corner_parallelogram_x,top_right_corner_parallelogram_y,STROKE_WIDTH);
        fprintf(fhd1,"\n</polyline>");
        
        fprintf(fhd1,"\n</g>");
        
        fprintf(fhd1,"\n</defs>");
        
        
        fprintf(fhd1, "\n\n<use xlink:href=\"#substrate layer B with outside circles\" transform=\"translate(%g,%g)\" />\n\n",sphere_radius,0.0);
        
        
        
        
        
        
        
    }
    
    
    
    // write the substrate layer C with circles everywhere
    
    if (NUMBER_OF_COLUMNS <= UPPER_LIMIT_FOR_SUBSTRATE_DISPLAY) {
        
        
        
        fprintf(fhd1, "\n<defs>\n");
        
        fprintf(fhd1, "<g id=\"C layer circle\"  style =\"stroke:rgb(192,192,192); stroke-width:%g; fill:rgb(192,192,192);stroke-opacity:0.7;fill-opacity:0.7;\" > \" \n",STROKE_WIDTH);
        
        fprintf(fhd1,"<circle cx=\"%g\" cy=\"%g\" r=\"%g\" />\n ",0.0,0.0,sphere_radius);
        
        fprintf(fhd1,"</g>\n");
        
        fprintf(fhd1, "</defs>\n");
        
        
        fprintf(fhd1, "\n<defs>\n");
        
        fprintf(fhd1,"<g id=\"substrate layer C with circles everywhere\">\n");
        
        
        
        
        
        for (i=1; i<=NUMBER_OF_ROWS; i++) {
            
            for (j=1; j<=NUMBER_OF_COLUMNS; j++){
                
                x_coordinate=(double)(delta_x+sphere_radius+((j-1)+(i-1)/2.0)*(sphere_radius*2));
                
                y_coordinate=(double) (delta_y+1*(sphere_radius+(i-1)*sphere_radius*sqrt(3)));
                
                y_coordinate = floor(y_coordinate*10)/10;
                
                
                fprintf(fhd1, "<use xlink:href=\"#C layer circle\" x=\"%g\" y=\"%g\"  />\n",x_coordinate,y_coordinate);
                
            }
            
        }
        
        //closing the group substrate layer C
        
        fprintf(fhd1,"</g>\n");
        
        // closing the the defintion of substrate layer C
        
        fprintf(fhd1,"\n</defs>\n");
        
        fprintf(fhd1,"\n");
        
        fprintf(fhd1,"\n");
        
        fprintf(fhd1,"\n");
        
        fprintf(fhd1, "<use xlink:href=\"#substrate layer C with circles everywhere\" transform=\"translate(%g,%g)\" />\n\n",sphere_radius,0.0);
        
        
    }
    
    
    
    // write the substrate layer C with outside circles and a parallelogramm inside
    
    if(NUMBER_OF_COLUMNS > UPPER_LIMIT_FOR_SUBSTRATE_DISPLAY){
        
        // write the substrate layer C with outside circles and a parallelogram inside
        
        fprintf(fhd1,"\n");
        
        fprintf(fhd1,"\n");
        
        fprintf(fhd1,"\n");
        
        fprintf(fhd1, "\n<defs>");
        
        fprintf(fhd1, "\n<g id=\"C layer circle\"  style =\"stroke:rgb(192,192,192); stroke-width:%g; fill:rgb(192,192,192);stroke-opacity:0.7;fill-opacity:0.7;\" > ",STROKE_WIDTH);
        
        fprintf(fhd1,"\n<circle cx=\"%g\" cy=\"%g\" r=\"%g\" /> ",0.0,0.0,sphere_radius);
        
        fprintf(fhd1,"\n</g>");
        
        fprintf(fhd1, "\n</defs>");

        fprintf(fhd1,"\n");
        
        fprintf(fhd1,"\n");
        
        fprintf(fhd1, "\n<defs>");
        
        fprintf(fhd1,"\n<g id=\"substrate layer C with outside circles\">");
        
        
        
        
        
        
        
        
        
        
        for (i=1; i<=NUMBER_OF_ROWS; i++) {
            
            for (j=1; j<=NUMBER_OF_COLUMNS; j++){
                
                
                x_coordinate=(double)(delta_x+sphere_radius+((j-1)+(i-1)/2.0)*(sphere_radius*2));
                
                y_coordinate=(double) (delta_y+1*(sphere_radius+(i-1)*sphere_radius*sqrt(3)));
                
                y_coordinate = floor(y_coordinate*10)/10;
                
                
                
                if (i==1 || i == NUMBER_OF_ROWS || j==1 || j == NUMBER_OF_COLUMNS) {
                    
                    
                    
                    fprintf(fhd1, "\n<use xlink:href=\"#C layer circle\" x=\"%g\" y=\"%g\"  />",x_coordinate,y_coordinate);
                    
                    
                    
                }
                
                if(i==1 && j==1){
                    
                    top_left_corner_parallelogram_x = x_coordinate;
                    top_left_corner_parallelogram_y = y_coordinate;
                    
                }
                
                
                if(i==1 && j==NUMBER_OF_COLUMNS){
                    
                    top_right_corner_parallelogram_x = x_coordinate;
                    top_right_corner_parallelogram_y = y_coordinate;
                    
                    
                }
                
                if(i==NUMBER_OF_ROWS && j==1){
                    
                    bottom_left_corner_parallelogram_x = x_coordinate;
                    bottom_left_corner_parallelogram_y = y_coordinate;
                    
                }
                
                
                if(i==NUMBER_OF_ROWS && j==NUMBER_OF_COLUMNS){
                    
                    bottom_right_corner_parallelogram_x = x_coordinate;
                    bottom_right_corner_parallelogram_y = y_coordinate;
                    
                }
                
                
                
                
            }
            
        }
        
        fprintf(fhd1,"\n");
        
        fprintf(fhd1,"\n");
        
        fprintf(fhd1, "\n<!-- the parallelogramm for layer C -->");
        
        fprintf(fhd1,"\n");
        
        fprintf(fhd1,"\n");
        
        
        
        fprintf(fhd1, "\n<polyline points= \"%g %g, %g %g,%g %g,%g %g \" style = \"stroke:rgb(192,192,192); stroke-width:%g; fill:rgb(192,192,192);stroke-opacity:0.7;fill-opacity:0.7;\">",top_left_corner_parallelogram_x,top_left_corner_parallelogram_y,bottom_left_corner_parallelogram_x,bottom_left_corner_parallelogram_y,bottom_right_corner_parallelogram_x,bottom_right_corner_parallelogram_y,top_right_corner_parallelogram_x,top_right_corner_parallelogram_y,STROKE_WIDTH);
        
        fprintf(fhd1,"\n</polyline>");
        
        
        
        //closing the group substrate layer C
        fprintf(fhd1,"\n</g>");
        // closing the the defintion of substrate layer C
        fprintf(fhd1,"\n</defs>");
        
        fprintf(fhd1,"\n");
        
        fprintf(fhd1,"\n");
        
        
        
        
        fprintf(fhd1, "\n<use xlink:href=\"#substrate layer C with outside circles\" transform=\"translate(%g,%g)\" />",+sphere_radius,0.0);
        
        
        
        
    }
    
    
    
    
    
    // The following double for loop writes the adsorbate matrix
    
    // The adsorbate lattice has to be shifted versus substrate layer C
    
    
    
    
    fprintf(fhd1,"\n");

    fprintf(fhd1,"\n");
    
    fprintf(fhd1,"\n");
    
    
    fprintf(fhd1, "\n<defs>");
    
    fprintf(fhd1, "\n<g id=\"adsorbate layer circle\"  style =\"stroke:rgb(255,255,0); stroke-width:%g; fill:rgb(255,255,0);stroke-opacity:1.0;fill-opacity:1.0;\"> ",STROKE_WIDTH);
    
    fprintf(fhd1,"\n<circle cx=\"%g\" cy=\"%g\" r=\"%g\" /> ",0.0,0.0,sphere_radius);
    
    
    fprintf(fhd1,"\n</g>");
    
    fprintf(fhd1, "\n</defs>");
    
    fprintf(fhd1,"\n");
    
    
    fprintf(fhd1, "\n<defs>");
    
    
    
    
    fprintf(fhd1, "\n<g id=\"adsorbate layer\">");
    
   
    
   
    
    
    
    
    for (i=1; i<=NUMBER_OF_ROWS; i++) {
        
        for (j=1; j<=NUMBER_OF_COLUMNS; j++){
            
            if(free_occupied_detector(i, j, &mcs)==1){
                
                x_coordinate=(double)(2*delta_x+sphere_radius+((j-1)+(i-1)/2.0)*(sphere_radius*2));
                // the upper line is the shift of each row with index i >1
                
                
                y_coordinate=(double) (2*delta_y+1*(sphere_radius+1*(i-1)*sphere_radius*sqrt(3)));
                
                y_coordinate = floor(y_coordinate*10)/10;
                
                
                fprintf(fhd1, "\n<use xlink:href=\"#adsorbate layer circle\" x=\"%g\" y=\"%g\"  />",x_coordinate,y_coordinate);
                
                
                
                
            }
            
        }
        
        
    }
    
    
    fprintf(fhd1,"\n</g>");
    
    fprintf(fhd1,"\n</defs>");
    
    fprintf(fhd1,"\n");
    
    fprintf(fhd1,"\n");
    
    fprintf(fhd1,"\n");
    
    
    fprintf(fhd1,"\n<!-- Jetzt wird die gesamte Adsorbat schicht verschoben  abcder-->");
    
    fprintf(fhd1, "\n<use xlink:href=\"#adsorbate layer\" transform=\"translate(%g,%g)\" />",sphere_radius,0.0);
  
    fprintf(fhd1,"\n");
    
    fprintf(fhd1,"\n");
    
    fprintf(fhd1,"\n");
    
    
    
    
    fprintf(fhd1,"<!-- Now we must write the legend      -->");
    
    
    // the circles for the legend
    
    fprintf(fhd1, "\n<defs>");
    
    
    fprintf(fhd1, "\n<g id=\"B layer circle for legend\"  style =\"stroke:rgb(64,64,64); stroke-width:%g; fill:rgb(64,64,64);stroke-opacity:1.0;fill-opacity:1.0;\" > \" ",STROKE_WIDTH);
    
    fprintf(fhd1,"\n<circle cx=\"%g\" cy=\"%g\" r=\"%g\" /> ",0.0,0.0,(double)(CIRCLE_RADIUS_LEGEND));
    
    
    fprintf(fhd1,"\n</g>");
    
    fprintf(fhd1, "\n<g id=\"C layer circle for legend\"  style =\"stroke:rgb(192,192,192); fill:rgb(192,192,192);\" >  ");
    
    fprintf(fhd1,"\n<circle cx=\"%g\" cy=\"%g\" r=\"%g\" /> ",0.0,0.0,(double)(CIRCLE_RADIUS_LEGEND));
    
    
    fprintf(fhd1,"\n</g>\n");
    
    fprintf(fhd1, "\n<g id=\"adsorbate layer circle for legend\"  style =\"stroke:rgb(255,255,0);\" > \" \n");
    
    
    
    fprintf(fhd1,"<circle cx=\"%g\" cy=\"%g\" r=\"%g\" /> ",0.0,0.0,(double)(CIRCLE_RADIUS_LEGEND) );
    
    fprintf(fhd1,"\n</g>");
    
    
    fprintf(fhd1, "\n</defs>");
    
    fprintf(fhd1,"\n");
    
    fprintf(fhd1,"\n");
    
    fprintf(fhd1,"\n");
    
    fprintf(fhd1,"\n");
    
    
    
    
    fprintf(fhd1, "\n<use xlink:href=\"#B layer circle for legend\" x=\"%g\" y=\"%g\" />\n",line_spacing/2.0+45.0,page_height-6.0*line_spacing-2.5);
    
    fprintf(fhd1, "\n<use xlink:href=\"#C layer circle for legend\" x=\"%g\" y=\"%g\" />\n",line_spacing/2.0+45.0,page_height-5.0*line_spacing-2.5);
    
    fprintf(fhd1, "\n<use xlink:href=\"#adsorbate layer circle for legend\" x=\"%g\" y=\"%g\" />\n",line_spacing/2.0+45.0,page_height-4.0*line_spacing-2.5);
    
    
    
    
    
    
    
    
    
    
    fprintf(fhd1,"\n\n<!-- The legend here all for Time Temperatur and Number of Adsorbates -->\n\n");
    
    
    fprintf(fhd1,"\n<g style=\"font-size: %ipt; font-weight:bold; font-family:sans-serif;\"> ",FONT_SIZE_LEGEND);
    
    fprintf(fhd1,"\n\n<text x=\"%g\" y=\"%g\">",line_spacing/2.0,page_height-6.0*line_spacing);
    
    fprintf(fhd1,"\n    Layer B");
    
    fprintf(fhd1,"\n</text>");
    
    fprintf(fhd1,"\n\n<text x=\"%g\" y=\"%g\">",line_spacing/2.0,page_height-5.0*line_spacing);
    
    fprintf(fhd1,"\n    Layer C");
    
    fprintf(fhd1,"\n</text>");
    
    fprintf(fhd1,"\n\n<text x=\"%g\" y=\"%g\">",line_spacing/2.0,page_height-4.0*line_spacing);
    
    fprintf(fhd1,"\n    Adsorbate");
    
    fprintf(fhd1,"\n</text>");
    
    
    
    fprintf(fhd1,"\n\n <text x=\"%g\" y=\"%g\" >",line_spacing/2.0-sphere_radius,page_height-3.0*line_spacing);
    
    fprintf(fhd1,"\nTime:   %6.3g sec\n",(double)(((mcs.line_in_experiment_monitor_file-1)*1.0)/FRAME_RATE));
    
    fprintf(fhd1,"</text>\n");
    
    
    fprintf(fhd1,"\n\n<text x=\"%g\" y=\"%g\" >",line_spacing/2.0-sphere_radius,page_height-2.0*line_spacing);
    
    fprintf(fhd1,"\nTemperature:   %3i K", (int) (trunc(mcs.temperature)));
    
    fprintf(fhd1,"\n</text>");
    
    
    fprintf(fhd1,"\n\n <text x=\"%g\" y=\"%g\" >",line_spacing/2.0-sphere_radius,page_height-1.0*line_spacing);
    
    fprintf(fhd1,"\nNumber of adsorbates:   %3i", mcs.number_of_adsorbates);
    
    fprintf(fhd1,"\n</text>");
    
    
    
    fprintf(fhd1, "\n\n</g>");
    
    
    
    
    
    
    
    
    
    
    fprintf(fhd1, "\n\n</svg>\n\n");
    
    
    fclose(fhd1);
    
    
    
    
    
}



void data_output_svg_ad_id(struct MCS_STRUCT *ptr_to_mcs_struct){
    
    // this procedure writes a file of the adsorbate structure in svg format
    
    ptr_to_mcs_struct=&mcs;
    
    FILE *fhd1;
    
    int i,j,k;
    
    int ad_id=0;
    
    int temperature_growth=TEMPERATURE_DURING_GROWTH;
    
    char local_filename[200]="";
    
    char number[200];
    
    double x_coordinate=0.0,y_coordinate=0.0,delta_x=0.0,delta_y=0.0;
    
    int font_size_legend=9;
    
    double line_spacing=1.5*font_size_legend;
    
    //  a double column figure in a journal typically has a width of 16 cm
    //  this means 16/2.54 inches
    //  Postscript default resolution 72 points pro inch
    
    double final_page_width=16./2.54*72;;
    
    double tentative_page_height=final_page_width/(sqrt(2.));
    
    double relative_sphere_radius=1.0;
    
    double sphere_radius=tentative_page_height/(2.*relative_sphere_radius+NUMBER_OF_ROWS*sqrt(3)*relative_sphere_radius);
    
    sphere_radius = floor(sphere_radius*10)/10;
    
    //double y_value_lowest_row=font_size_legend*1;
    
    double test_bottom_right_y=sphere_radius*(2.-sqrt(3)/3.+sqrt(3)*NUMBER_OF_ROWS);
    
    double test_bottom_right_x=4*sphere_radius+(NUMBER_OF_COLUMNS+NUMBER_OF_ROWS/2.-3./2.)*sphere_radius*2;
    
    double measured_page_height=test_bottom_right_y;
    
    double measured_page_width=test_bottom_right_x;
    
    double correctur_factor=measured_page_width/measured_page_height;
    
    double page_width=final_page_width;
    
    double page_height=page_width/correctur_factor;
    
    
    // recalculation of the sphere radius
    
    
    sphere_radius=page_height/(2.*relative_sphere_radius+NUMBER_OF_ROWS*sqrt(3)*relative_sphere_radius);
    
    sphere_radius = floor(sphere_radius*10)/10;
    
    
    // Layer C is shifted towards layer B //
    
    // The shift in x-direction is called delta_x , its value is sphere_radius
    // The shift in x-direction is called delta_y , its value is minus sphere_radius times sqrt(3)/3
    // the later comes from the geometrical calculation that the layer C atom is just in the middle of
    // an equilateral triangle which is spanned by three atoms of the layer B (the layer below C)
    
    delta_x=sphere_radius;
    
    delta_y=sphere_radius*tan(M_PI/6);
    
    
    
    // in case of number of columns and rows larger than for example 150 do not write the layers B and C but a simple gray parallelogramm instead
    
    // define the parallelogramm which replaces layer c for greater matrices
    
    
    // seen from the top left corner of the page
    
    double
    
    top_left_corner_parallelogram_x=2*sphere_radius ,
    
    top_left_corner_parallelogram_y=-1.*sphere_radius*(1.+sqrt(3)/3.),
    
    bottom_left_corner_parallelogram_x=sphere_radius+sphere_radius*NUMBER_OF_ROWS,
    
    bottom_left_corner_parallelogram_y=(.1547005390*sphere_radius-1.732050808*sphere_radius*NUMBER_OF_ROWS),
    
    top_right_corner_parallelogram_x=2*sphere_radius*NUMBER_OF_COLUMNS,
    
    top_right_corner_parallelogram_y= top_left_corner_parallelogram_y, // ist ungeaendert
    
    bottom_right_corner_parallelogram_x=-1.*sphere_radius+2.*sphere_radius*NUMBER_OF_COLUMNS+sphere_radius*NUMBER_OF_ROWS,
    
    bottom_right_corner_parallelogram_y=(.1547005390*sphere_radius-1.732050808*sphere_radius*NUMBER_OF_ROWS);
    
    
    
    // The layers B, C and the adsorbate layer are shifted vs each other
    // by sphere_radius in x and by sphere_radius*tan(30 deg) in y
    
    
    int seconds_experiment=0,milli_seconds_experiment=0;
    
    
    if (mcs.mc_time<1) {
        
        milli_seconds_experiment=(int) (trunc(mcs.mc_time*1000)) ;
        
        
        sprintf(number,"steps_%09ld_T_initial_%03iK_T_actual_%03iK_time_%03ims_%04i_adsorbates",mcs.number_mc_step,temperature_growth,(int)(mcs.temperature),milli_seconds_experiment,mcs.number_of_adsorbates);
        
        
        
    }
    
    
    if (mcs.mc_time>1 && mcs.mc_time<=mcs.duration_of_experiment) {
        
        
        seconds_experiment=(int) (trunc(mcs.mc_time)) ;
        
        milli_seconds_experiment=(int) (trunc(mcs.mc_time*1000-seconds_experiment*1000)) ;
        
        sprintf(number,"steps_%09ld_T_initial_%03iK_T_actual_%03iK_time_%02is_%03ims_%04i_adsorbates",mcs.number_mc_step,temperature_growth,(int)(mcs.temperature),seconds_experiment,milli_seconds_experiment,mcs.number_of_adsorbates);
        
        
        
    }
    
    char number_new[20];
    
    sprintf(number_new,"m");
    
    char line_in_experiment_monitor_file[1000];
    
    sprintf(line_in_experiment_monitor_file,"%03d",mcs.line_in_experiment_monitor_file);
    
    strcat(number_new,line_in_experiment_monitor_file);
    
    
    strcat(local_filename,DATA_DIRECTORY);
    
    //strcat(local_filename,"adsorb_struc_");
    
    strcat(local_filename,number_new);
    
    strcat(local_filename,".svg");
    
    
    fhd1=fopen(local_filename,"w+");
    
    
    if (fhd1==NULL) {
        
        printf("\n\n Warning \n\n The file named \n -->%s<-- \ncould not be opened due to an error\nPlease check the line 100 of the  source code\n",local_filename);
        
        
    }
    
    
    else {
        
        printf("\n\nThe file with the name \n-->%s<--\nwas successfully opened \n",local_filename);
        
        
    }
    
    // some helpers for svg
    
    // The following lines are taken from w3c.org
    
    char svg_header_line_one[91]="<?xml version=\"1.0\" standalone=\"no\"?>";
    
    char svg_header_line_two[]="<!DOCTYPE svg PUBLIC \"-//W3C//DTD SVG 1.1//EN\" \"http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd\">";
    
    // line three contains the page size and is defined later
    
    char svg_header_line_three[200]="";
    
    char svg_header_line_four[]="xmlns=\"http://www.w3.org/2000/svg\"";
    
    char svg_header_line_five[]="xmlns:xlink=\"http://www.w3.org/1999/xlink\">";
    
    // you find a help for this here: https://stackoverflow.com/questions/29884930/svg-namespace-prefix-xlink-for-href-on-textpath-is-not-defined
    
    //  define the header line three
    
    //    sprintf(svg_header_line_three,"<svg  viewBox=\"%g %g %g %g\" width=\"%gmm\" height=\"%gmm\" version=\"1.1\" ",(X_OFFSET_PERCENT/100.*imag_par.page_width),(Y_OFFSET_PERCENT/100.*imag_par.page_height) ,(imag_par.page_width/MAGNIFICATION_IN_VIEW_BOX),(imag_par.page_height/MAGNIFICATION_IN_VIEW_BOX),imag_par.page_width,imag_par.page_height);
    
    
    sprintf(svg_header_line_three,"<svg  viewBox=\"0 0 %i %i\" width=\"%gmm\" height=\"%gmm\" version=\"1.1\" ",(int)(ceil(page_width)),(int)(ceil(page_height)),page_width,page_height);
    
    
    
    fprintf(stdout, "%s\n",local_filename);
    
    
    
    if (fhd1==NULL) {
        
        printf("Error in opening the file with name\n -%s- ",local_filename);
        
        printf("\n\nplease check line number 47 in the programm code \n may be add or remove a / ? \n\n ");
        
        
    }
    
    else {
        
        
        printf("\n\n A  svg file  named \n -->%s<-- \nwas successfully written \n ",local_filename);
        
        
    }
    
    
    
    fprintf(fhd1,"%s\n",svg_header_line_one);
    
    fprintf(fhd1,"%s\n",svg_header_line_two);
    
    fprintf(fhd1,"%s\n",svg_header_line_three);
    
    fprintf(fhd1,"%s\n",svg_header_line_four);
    
    fprintf(fhd1,"%s\n",svg_header_line_five);
    
    
    
    // write the substrate layer B with circles everywhere
    
    if(NUMBER_OF_COLUMNS <= UPPER_LIMIT_FOR_SUBSTRATE_DISPLAY) {
        
        // write the substrate layer B with circles everywhere
        
        fprintf(fhd1,"\n<!--write the substrate layer B with circles everywhere -->\n");
        
        
        
        fprintf(fhd1, "\n<defs>");
        
        fprintf(fhd1, "\n<g id=\"B layer circle\"  style =\"stroke:rgb(64,64,64); stroke-width:%g; fill:rgb(64,64,64);stroke-opacity:1.0;fill-opacity:1.0;\" > ",STROKE_WIDTH);
        
        fprintf(fhd1,"\n<circle cx=\"%g\" cy=\"%g\" r=\"%g\" /> ",0.0,0.0,sphere_radius);
        
        fprintf(fhd1,"\n</g>");
        
        fprintf(fhd1, "\n</defs>");
        
        fprintf(fhd1,"\n");
        
        fprintf(fhd1,"\n");
        
        fprintf(fhd1, "\n<defs>");
        
        fprintf(fhd1, "\n<g id=\"substrate layer B with circles everywhere\">");
        
        
        
        
        for (i=1; i<=NUMBER_OF_ROWS; i++) {
            
            for (j=1; j<=NUMBER_OF_COLUMNS; j++){
                
                x_coordinate=(double)(sphere_radius+((j-1)+(i-1)/2.0)*(sphere_radius*2));
                // the upper line is the shift of each row with index i >1
                
                
                y_coordinate=(double)((sphere_radius+1*((i-1)*sphere_radius*sqrt(3))));
                
                
                y_coordinate = floor(y_coordinate*10)/10;
                
                
                
                fprintf(fhd1, "\n<use xlink:href=\"#B layer circle\" x=\"%g\" y=\"%g\"  />",x_coordinate,y_coordinate);
                
                
                
                
                
                
            }
            
        }
        
        fprintf(fhd1,"\n</g>");
        
        fprintf(fhd1, "\n</defs>");
        
        fprintf(fhd1,"\n");
        
        fprintf(fhd1,"\n");
        
        fprintf(fhd1,"\n");
        
        
        
        fprintf(fhd1, "\n<use xlink:href=\"#substrate layer B with circles everywhere\" transform=\"translate(%g,%g)\" />",sphere_radius,0.0);
        
        
        fprintf(fhd1,"\n");
        
        fprintf(fhd1,"\n");
        
        fprintf(fhd1,"\n");
        
        
        
        
        
    }
    
    
    
    
    // write the substrate layer B with outside circles and a parallelogramm inside
    
    
    if(NUMBER_OF_COLUMNS > UPPER_LIMIT_FOR_SUBSTRATE_DISPLAY) {
        
        
        
        // write the substrate layer B
        
        fprintf(fhd1,"\n");
        
        fprintf(fhd1,"\n");
        
        
        fprintf(fhd1,"\n<defs>");
        
        fprintf(fhd1, "\n<g id=\"B layer circle\"  style =\"stroke:rgb(64,64,64); stroke-width:%g; fill:rgb(64,64,64);stroke-opacity:1.0;fill-opacity:1.0;\" > \" ",STROKE_WIDTH);
        
        fprintf(fhd1,"\n<circle cx=\"%g\" cy=\"%g\" r=\"%g\" />",0.0,0.0,sphere_radius);
        
        fprintf(fhd1,"\n</g>");
        
        fprintf(fhd1, "\n</defs>");
        
        
        fprintf(fhd1,"\n");
        
        fprintf(fhd1,"\n");
        
        
        fprintf(fhd1, "\n<defs> ");
        
        fprintf(fhd1, "\n<g id=\"substrate layer B with outside circles\" >");
        
        
        
        fprintf(fhd1,"\n<!-- Hier mal schauen  a and b  only -->\n");
        
        
        
        
        
        for (i=1; i<=NUMBER_OF_ROWS; i++) {
            
            for (j=1; j<=NUMBER_OF_COLUMNS; j++){
                
                x_coordinate=(double)(sphere_radius+((j-1)+(i-1)/2.0)*(sphere_radius*2));
                // the upper line is the shift of each row with index i >1
                
                x_coordinate = floor(x_coordinate*10)/10;
                
                
                y_coordinate=(double)((sphere_radius+1*((i-1)*sphere_radius*sqrt(3))));
                
                y_coordinate = floor(y_coordinate*10)/10;
                
                
                if (i==1 || i == NUMBER_OF_ROWS || j==1 || j == NUMBER_OF_COLUMNS) {
                    
                    
                    fprintf(fhd1, "<use xlink:href=\"#B layer circle\" x=\"%g\" y=\"%g\"  />\n",x_coordinate,y_coordinate);
                    
                }
                
                
                
                if(i==1 && j==1){
                    
                    top_left_corner_parallelogram_x = x_coordinate;
                    top_left_corner_parallelogram_y = y_coordinate;
                    
                }
                
                
                if(i==1 && j==NUMBER_OF_COLUMNS){
                    
                    top_right_corner_parallelogram_x = x_coordinate;
                    top_right_corner_parallelogram_y = y_coordinate;
                    
                    
                }
                
                if(i==NUMBER_OF_ROWS && j==1){
                    
                    bottom_left_corner_parallelogram_x = x_coordinate;
                    bottom_left_corner_parallelogram_y = y_coordinate;
                    
                }
                
                
                if(i==NUMBER_OF_ROWS && j==NUMBER_OF_COLUMNS){
                    
                    bottom_right_corner_parallelogram_x = x_coordinate;
                    bottom_right_corner_parallelogram_y = y_coordinate;
                    
                }
                
                
                
                
                
                
            }
            
        }
        
        
        fprintf(fhd1, "\n<!-- the parallelogramm for layer B -->\n");
        
        fprintf(fhd1, "\n<polyline points= \"%g %g, %g %g,%g %g,%g %g \" style = \"stroke:rgb(64,64,64); stroke-width:%g; fill:rgb(64,64,64);stroke-opacity:1.0;fill-opacity:1.0;\">",top_left_corner_parallelogram_x,top_left_corner_parallelogram_y,bottom_left_corner_parallelogram_x,bottom_left_corner_parallelogram_y,bottom_right_corner_parallelogram_x,bottom_right_corner_parallelogram_y,top_right_corner_parallelogram_x,top_right_corner_parallelogram_y,STROKE_WIDTH);
        fprintf(fhd1,"\n</polyline>");
        
        fprintf(fhd1,"\n</g>");
        
        fprintf(fhd1,"\n</defs>");
        
        
        fprintf(fhd1, "\n\n<use xlink:href=\"#substrate layer B with outside circles\" transform=\"translate(%g,%g)\" />\n\n",sphere_radius,0.0);
        
        
        
        
        
        
        
    }
    
    
    
    // write the substrate layer C with circles everywhere
    
    if (NUMBER_OF_COLUMNS <= UPPER_LIMIT_FOR_SUBSTRATE_DISPLAY) {
        
        
        
        fprintf(fhd1, "\n<defs>\n");
        
        fprintf(fhd1, "<g id=\"C layer circle\"  style =\"stroke:rgb(192,192,192); stroke-width:%g; fill:rgb(192,192,192);stroke-opacity:0.7;fill-opacity:0.7;\" > \" \n",STROKE_WIDTH);
        
        fprintf(fhd1,"<circle cx=\"%g\" cy=\"%g\" r=\"%g\" />\n ",0.0,0.0,sphere_radius);
        
        fprintf(fhd1,"</g>\n");
        
        fprintf(fhd1, "</defs>\n");
        
        
        fprintf(fhd1, "\n<defs>\n");
        
        fprintf(fhd1,"<g id=\"substrate layer C with circles everywhere\">\n");
        
        
        
        
        
        for (i=1; i<=NUMBER_OF_ROWS; i++) {
            
            for (j=1; j<=NUMBER_OF_COLUMNS; j++){
                
                x_coordinate=(double)(delta_x+sphere_radius+((j-1)+(i-1)/2.0)*(sphere_radius*2));
                
                y_coordinate=(double) (delta_y+1*(sphere_radius+(i-1)*sphere_radius*sqrt(3)));
                
                y_coordinate = floor(y_coordinate*10)/10;
                
                
                fprintf(fhd1, "<use xlink:href=\"#C layer circle\" x=\"%g\" y=\"%g\"  />\n",x_coordinate,y_coordinate);
                
            }
            
        }
        
        //closing the group substrate layer C
        
        fprintf(fhd1,"</g>\n");
        
        // closing the the defintion of substrate layer C
        
        fprintf(fhd1,"\n</defs>\n");
        
        fprintf(fhd1,"\n");
        
        fprintf(fhd1,"\n");
        
        fprintf(fhd1,"\n");
        
        fprintf(fhd1, "<use xlink:href=\"#substrate layer C with circles everywhere\" transform=\"translate(%g,%g)\" />\n\n",sphere_radius,0.0);
        
        
    }
    
    
    
    // write the substrate layer C with outside circles and a parallelogramm inside
    
    if(NUMBER_OF_COLUMNS > UPPER_LIMIT_FOR_SUBSTRATE_DISPLAY){
        
        // write the substrate layer C with outside circles and a parallelogram inside
        
        fprintf(fhd1,"\n");
        
        fprintf(fhd1,"\n");
        
        fprintf(fhd1,"\n");
        
        fprintf(fhd1, "\n<defs>");
        
        fprintf(fhd1, "\n<g id=\"C layer circle\"  style =\"stroke:rgb(192,192,192); stroke-width:%g; fill:rgb(192,192,192);stroke-opacity:0.7;fill-opacity:0.7;\" > ",STROKE_WIDTH);
        
        fprintf(fhd1,"\n<circle cx=\"%g\" cy=\"%g\" r=\"%g\" /> ",0.0,0.0,sphere_radius);
        
        fprintf(fhd1,"\n</g>");
        
        fprintf(fhd1, "\n</defs>");
        
        fprintf(fhd1,"\n");
        
        fprintf(fhd1,"\n");
        
        fprintf(fhd1, "\n<defs>");
        
        fprintf(fhd1,"\n<g id=\"substrate layer C with outside circles\">");
        
        
        
        
        
        
        
        
        
        
        for (i=1; i<=NUMBER_OF_ROWS; i++) {
            
            for (j=1; j<=NUMBER_OF_COLUMNS; j++){
                
                
                x_coordinate=(double)(delta_x+sphere_radius+((j-1)+(i-1)/2.0)*(sphere_radius*2));
                
                y_coordinate=(double) (delta_y+1*(sphere_radius+(i-1)*sphere_radius*sqrt(3)));
                
                y_coordinate = floor(y_coordinate*10)/10;
                
                
                
                if (i==1 || i == NUMBER_OF_ROWS || j==1 || j == NUMBER_OF_COLUMNS) {
                    
                    
                    
                    fprintf(fhd1, "\n<use xlink:href=\"#C layer circle\" x=\"%g\" y=\"%g\"  />",x_coordinate,y_coordinate);
                    
                    
                    
                }
                
                if(i==1 && j==1){
                    
                    top_left_corner_parallelogram_x = x_coordinate;
                    top_left_corner_parallelogram_y = y_coordinate;
                    
                }
                
                
                if(i==1 && j==NUMBER_OF_COLUMNS){
                    
                    top_right_corner_parallelogram_x = x_coordinate;
                    top_right_corner_parallelogram_y = y_coordinate;
                    
                    
                }
                
                if(i==NUMBER_OF_ROWS && j==1){
                    
                    bottom_left_corner_parallelogram_x = x_coordinate;
                    bottom_left_corner_parallelogram_y = y_coordinate;
                    
                }
                
                
                if(i==NUMBER_OF_ROWS && j==NUMBER_OF_COLUMNS){
                    
                    bottom_right_corner_parallelogram_x = x_coordinate;
                    bottom_right_corner_parallelogram_y = y_coordinate;
                    
                }
                
                
                
                
            }
            
        }
        
        fprintf(fhd1,"\n");
        
        fprintf(fhd1,"\n");
        
        fprintf(fhd1, "\n<!-- the parallelogramm for layer C -->");
        
        fprintf(fhd1,"\n");
        
        fprintf(fhd1,"\n");
        
        
        
        fprintf(fhd1, "\n<polyline points= \"%g %g, %g %g,%g %g,%g %g \" style = \"stroke:rgb(192,192,192); stroke-width:%g; fill:rgb(192,192,192);stroke-opacity:0.7;fill-opacity:0.7;\">",top_left_corner_parallelogram_x,top_left_corner_parallelogram_y,bottom_left_corner_parallelogram_x,bottom_left_corner_parallelogram_y,bottom_right_corner_parallelogram_x,bottom_right_corner_parallelogram_y,top_right_corner_parallelogram_x,top_right_corner_parallelogram_y,STROKE_WIDTH);
        
        fprintf(fhd1,"\n</polyline>");
        
        
        
        //closing the group substrate layer C
        fprintf(fhd1,"\n</g>");
        
        // closing the the defintion of substrate layer C
        fprintf(fhd1,"\n</defs>");
        
        fprintf(fhd1,"\n");
        
        fprintf(fhd1,"\n");
        
        
        
        
        fprintf(fhd1, "\n<use xlink:href=\"#substrate layer C with outside circles\" transform=\"translate(%g,%g)\" />",+sphere_radius,0.0);
        
        
        
        
    }
    
    
    
    
    
    // The following double for loop writes the adsorbate matrix
    
    // The adsorbate lattice has to be shifted versus substrate layer C
    
    
    
    
    fprintf(fhd1,"\n");
    
    fprintf(fhd1,"\n");
    
    fprintf(fhd1,"\n");
    
    
    fprintf(fhd1, "\n<defs>");
    
    fprintf(fhd1, "\n<g id=\"adsorbate layer circle\"  style =\"stroke:rgb(255,255,0); stroke-width:%g; fill:rgb(255,255,0);stroke-opacity:1.0;fill-opacity:1.0;\"> ",STROKE_WIDTH);
    
    fprintf(fhd1,"\n<circle cx=\"%g\" cy=\"%g\" r=\"%g\" /> ",0.0,0.0,sphere_radius);
    
    
    fprintf(fhd1,"\n</g>");
    
    fprintf(fhd1, "\n</defs>");
    
    fprintf(fhd1,"\n");
    
    
    fprintf(fhd1, "\n<defs>");
    
    
    
    
    fprintf(fhd1, "\n<g id=\"adsorbate layer\">");
    
    
    
    
    
    
    
    
    for (i=1; i<=NUMBER_OF_ROWS; i++) {
        
        for (j=1; j<=NUMBER_OF_COLUMNS; j++){
            
            if(free_occupied_detector(i, j, &mcs)==1){
                
                for (k=1; k<=mcs.number_of_adsorbates*6; k++) {
                    
                    if (mcs.table_of_adsorbate_data[k][2]==i && mcs.table_of_adsorbate_data[k][3]==j  ) {
                        ad_id=mcs.table_of_adsorbate_data[k][1];
                    }
                }
                
                    
                    x_coordinate=(double)(2*delta_x+sphere_radius+((j-1)+(i-1)/2.0)*(sphere_radius*2));
                    // the upper line is the shift of each row with index i >1
                    
                    
                    y_coordinate=(double) (2*delta_y+1*(sphere_radius+1*(i-1)*sphere_radius*sqrt(3)));
                    
                    y_coordinate = floor(y_coordinate*10)/10;
                    
                    
                    fprintf(fhd1, "\n<use xlink:href=\"#adsorbate layer circle\" x=\"%g\" y=\"%g\"  />",x_coordinate,y_coordinate);
                    
                fprintf(fhd1,"\n<text x=\"%g\" y=\"%g\" fill=\"black\" font-size=\"0.5\">%d</text> ",x_coordinate-sphere_radius*0.6,y_coordinate+sphere_radius/4,ad_id);
                    
                    
                }
                
            }
            
            
        }
        
        
        fprintf(fhd1,"\n</g>");
        
        fprintf(fhd1,"\n</defs>");
        
        fprintf(fhd1,"\n");
        
        fprintf(fhd1,"\n");
        
        fprintf(fhd1,"\n");
        
        
        fprintf(fhd1,"\n<!-- Jetzt wird die gesamte Adsorbat schicht verschoben  abcder-->");
        
        fprintf(fhd1, "\n<use xlink:href=\"#adsorbate layer\" transform=\"translate(%g,%g)\" />",sphere_radius,0.0);
        
        fprintf(fhd1,"\n");
        
        fprintf(fhd1,"\n");
        
        fprintf(fhd1,"\n");
        
        
        
        
        fprintf(fhd1,"<!-- Now we must write the legend      -->");
        
        
        // the circles for the legend
        
        fprintf(fhd1, "\n<defs>");
        
        
        fprintf(fhd1, "\n<g id=\"B layer circle for legend\"  style =\"stroke:rgb(64,64,64); stroke-width:%g; fill:rgb(64,64,64);stroke-opacity:1.0;fill-opacity:1.0;\" > \" ",STROKE_WIDTH);
        
        fprintf(fhd1,"\n<circle cx=\"%g\" cy=\"%g\" r=\"%g\" /> ",0.0,0.0,(double)(CIRCLE_RADIUS_LEGEND));
        
        
        fprintf(fhd1,"\n</g>");
        
        fprintf(fhd1, "\n<g id=\"C layer circle for legend\"  style =\"stroke:rgb(192,192,192); fill:rgb(192,192,192);\" >  ");
        
        fprintf(fhd1,"\n<circle cx=\"%g\" cy=\"%g\" r=\"%g\" /> ",0.0,0.0,(double)(CIRCLE_RADIUS_LEGEND));
        
        
        fprintf(fhd1,"\n</g>\n");
        
        fprintf(fhd1, "\n<g id=\"adsorbate layer circle for legend\"  style =\"stroke:rgb(255,255,0);\" > \" \n");
        
        
        
        fprintf(fhd1,"<circle cx=\"%g\" cy=\"%g\" r=\"%g\" /> ",0.0,0.0,(double)(CIRCLE_RADIUS_LEGEND) );
        
        fprintf(fhd1,"\n</g>");
        
        
        fprintf(fhd1, "\n</defs>");
        
        fprintf(fhd1,"\n");
        
        fprintf(fhd1,"\n");
        
        fprintf(fhd1,"\n");
        
        fprintf(fhd1,"\n");
        
        
        
        
        fprintf(fhd1, "\n<use xlink:href=\"#B layer circle for legend\" x=\"%g\" y=\"%g\" />\n",line_spacing/2.0+45.0,page_height-6.0*line_spacing-2.5);
        
        fprintf(fhd1, "\n<use xlink:href=\"#C layer circle for legend\" x=\"%g\" y=\"%g\" />\n",line_spacing/2.0+45.0,page_height-5.0*line_spacing-2.5);
        
        fprintf(fhd1, "\n<use xlink:href=\"#adsorbate layer circle for legend\" x=\"%g\" y=\"%g\" />\n",line_spacing/2.0+45.0,page_height-4.0*line_spacing-2.5);
        
        
        
        
        
        
        
        
        
        
        fprintf(fhd1,"\n\n<!-- The legend here all for Time Temperatur and Number of Adsorbates -->\n\n");
        
        
        fprintf(fhd1,"\n<g style=\"font-size: %ipt; font-weight:bold; font-family:sans-serif;\"> ",FONT_SIZE_LEGEND);
        
        fprintf(fhd1,"\n\n<text x=\"%g\" y=\"%g\">",line_spacing/2.0,page_height-6.0*line_spacing);
        
        fprintf(fhd1,"\n    Layer B");
        
        fprintf(fhd1,"\n</text>");
        
        fprintf(fhd1,"\n\n<text x=\"%g\" y=\"%g\">",line_spacing/2.0,page_height-5.0*line_spacing);
        
        fprintf(fhd1,"\n    Layer C");
        
        fprintf(fhd1,"\n</text>");
        
        fprintf(fhd1,"\n\n<text x=\"%g\" y=\"%g\">",line_spacing/2.0,page_height-4.0*line_spacing);
        
        fprintf(fhd1,"\n    Adsorbate");
        
        fprintf(fhd1,"\n</text>");
        
        
        
        fprintf(fhd1,"\n\n <text x=\"%g\" y=\"%g\" >",line_spacing/2.0-sphere_radius,page_height-3.0*line_spacing);
        
        fprintf(fhd1,"\nTime:   %6.3g sec\n",(double)(((mcs.line_in_experiment_monitor_file-1)*1.0)/FRAME_RATE));
        
        fprintf(fhd1,"</text>\n");
        
        
        fprintf(fhd1,"\n\n<text x=\"%g\" y=\"%g\" >",line_spacing/2.0-sphere_radius,page_height-2.0*line_spacing);
        
        fprintf(fhd1,"\nTemperature:   %3i K", (int) (trunc(mcs.temperature)));
        
        fprintf(fhd1,"\n</text>");
        
        
        fprintf(fhd1,"\n\n <text x=\"%g\" y=\"%g\" >",line_spacing/2.0-sphere_radius,page_height-1.0*line_spacing);
        
        fprintf(fhd1,"\nNumber of adsorbates:   %3i", mcs.number_of_adsorbates);
        
        fprintf(fhd1,"\n</text>");
        
        
        
        fprintf(fhd1, "\n\n</g>");
        
        
        
        fprintf(fhd1, "\n\n</svg>\n\n");
        
        
        fclose(fhd1);
        
        
        
        
        
    }






void data_output_animated_svg(struct MCS_STRUCT *ptr_to_mcs_struct){
    
    // this procedure is planned to write a sequence of adsorbate structures as animated svg.
    
    ptr_to_mcs_struct=&mcs;
    
    FILE *fhd1;
    
    int i=0,j=0,index_in_history_adsorbate_positions=0;
    
    int i_coor=0,j_coor=0;
    
    int index_timed_output=0;
    
    char filename_animated_svg[200]="";
    
    double x_coordinate=0.0,y_coordinate=0.0,delta_x=0.0,delta_y=0.0;
    
    int font_size_legend=9;
    
    double line_spacing=1.5*font_size_legend;
    
    //  a double column figure in a journal typically has a width of 16 cm
    //  this means 16/2.54 inches
    //  Postscript default resolution 72 points pro inch
    
    double final_page_width=16./2.54*72;;
    
    double tentative_page_height=final_page_width/(sqrt(2.));
    
    double relative_sphere_radius=1.0;
    
    double sphere_radius=tentative_page_height/(2.*relative_sphere_radius+NUMBER_OF_ROWS*sqrt(3)*relative_sphere_radius);
    
    sphere_radius = floor(sphere_radius*10)/10;
    
    //   double y_value_lowest_row=font_size_legend*1;
    
    double test_bottom_right_y=sphere_radius*(2.-sqrt(3)/3.+sqrt(3)*NUMBER_OF_ROWS);
    
    double test_bottom_right_x=4*sphere_radius+(NUMBER_OF_COLUMNS+NUMBER_OF_ROWS/2.-3./2.)*sphere_radius*2;
    
    double measured_page_height=test_bottom_right_y;
    
    double measured_page_width=test_bottom_right_x;
    
    double correctur_factor=measured_page_width/measured_page_height;
    
    double page_width=final_page_width;
    
    double page_height=page_width/correctur_factor;
    
    
    
    // recalculation of the sphere radius
    
    
    sphere_radius=page_height/(2.*relative_sphere_radius+NUMBER_OF_ROWS*sqrt(3)*relative_sphere_radius);
    
    sphere_radius = floor(sphere_radius*10)/10;
    
    // Layer C is shifted towards layer B //
    
    // The shift in x-direction is called delta_x , its value is sphere_radius
    // The shift in y-direction is called delta_y , its value is minus sphere_radius times sqrt(3)/3
    // the later comes from the geometrical calculation that the layer C atom is just in the middle of
    // an equilateral triangle which is spanned by three atoms of the layer B (the layer below C)
    
    delta_x=sphere_radius;
    
    delta_y=sphere_radius*tan(M_PI/6);
    
    //   double x_coordinate_actual_position_legend=0.0,y_coordinate_actual_position_legend=0.0;
    
    
    // in case of number of columns and rows larger than for example 150 do not write the layers B and C but a simple gray parallelogramm instead
    
    // define the parallelogramm which replaces layer c for greater matrices
    
    
    // seen from the top left corner of the page
    
    double
    
    top_left_corner_parallelogram_x=2*sphere_radius ,
    
    top_left_corner_parallelogram_y=-1.*sphere_radius*(1.+sqrt(3)/3.),
    
    bottom_left_corner_parallelogram_x=sphere_radius+sphere_radius*NUMBER_OF_ROWS,
    
    bottom_left_corner_parallelogram_y=(.1547005390*sphere_radius-1.732050808*sphere_radius*NUMBER_OF_ROWS),
    
    top_right_corner_parallelogram_x=2*sphere_radius*NUMBER_OF_COLUMNS,
    
    top_right_corner_parallelogram_y= top_left_corner_parallelogram_y, // ist ungeaendert
    
    bottom_right_corner_parallelogram_x=-1.*sphere_radius+2.*sphere_radius*NUMBER_OF_COLUMNS+sphere_radius*NUMBER_OF_ROWS,
    
    bottom_right_corner_parallelogram_y=(.1547005390*sphere_radius-1.732050808*sphere_radius*NUMBER_OF_ROWS);
    
    
    
    // The layers B, C and the adsorbate layer are shifted vs each other
    // by sphere_radius in x and by sphere_radius*tan(30 deg) in y
    
    
    
    strcat(filename_animated_svg,DATA_DIRECTORY);
    
    strcat(filename_animated_svg,"adsorbate_structure_animated");
    
    strcat(filename_animated_svg,".svg");
    
    fhd1=fopen(filename_animated_svg,"w+");
    
    
    if (fhd1==NULL) {
        
        printf("\n\n Warning \n\n The file named \n -->%s<-- \ncould not be opened due to an error\nPlease check the line 100 of the  source code\n",filename_animated_svg);
        
        
    }
    
    
    else {
        
        printf("\n\nThe file with the name \n-->%s<--\nwas successfully opened \n",filename_animated_svg);
        
        
    }
    
    
    
    // some helpers for svg
    
    // The following lines are taken from w3c.org
    
    char svg_header_line_one[91]="<?xml version=\"1.0\" standalone=\"no\"?>";
    
    char svg_header_line_two[]="<!DOCTYPE svg PUBLIC \"-//W3C//DTD SVG 1.1//EN\" \"http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd\">";
    
    // line three contains the page size and is defined later
    
    char svg_header_line_three[200]="";
    
    char svg_header_line_four[]="xmlns=\"http://www.w3.org/2000/svg\"";
    
    char svg_header_line_five[]="xmlns:xlink=\"http://www.w3.org/1999/xlink\">";
    
    // you find a help for this here: https://stackoverflow.com/questions/29884930/svg-namespace-prefix-xlink-for-href-on-textpath-is-not-defined
    
    //  define the header line three
    
    //    sprintf(svg_header_line_three,"<svg  viewBox=\"%g %g %g %g\" width=\"%gmm\" height=\"%gmm\" version=\"1.1\" ",(X_OFFSET_PERCENT/100.*imag_par.page_width),(Y_OFFSET_PERCENT/100.*imag_par.page_height) ,(imag_par.page_width/MAGNIFICATION_IN_VIEW_BOX),(imag_par.page_height/MAGNIFICATION_IN_VIEW_BOX),imag_par.page_width,imag_par.page_height);
    
    
    sprintf(svg_header_line_three,"<svg  viewBox=\"0 0 %i %i\" width=\"%gmm\" height=\"%gmm\" version=\"1.1\" ",(int)(ceil(page_width)),(int)(ceil(page_height)),page_width,page_height);
    
    
    
    //fprintf(stdout, "%s\n",filename_animated_svg);
    
    
    
    
    
    
    fprintf(fhd1,"%s\n",svg_header_line_one);
    
    fprintf(fhd1,"%s\n",svg_header_line_two);
    
    fprintf(fhd1,"%s\n",svg_header_line_three);
    
    fprintf(fhd1,"%s\n",svg_header_line_four);
    
    fprintf(fhd1,"%s\n",svg_header_line_five);
    
    
    
    
    
    
    
    
    
    
    //  The layers B, C and the adsorbate layer are shifted vs each other
    
    //  by sphere_radius in x and by sphere_radius*tan(30 deg) in y
    
    //  This is the reason for the above definition of
    
    //    double  delta_x=sphere_radius;
    
    //  double  delta_y=sphere_radius*tan(M_PI/6);
    
    
    
    
    // write the substrate layer B with circles everywhere
    
    if(NUMBER_OF_COLUMNS <= UPPER_LIMIT_FOR_SUBSTRATE_DISPLAY) {
        
        // write the substrate layer B with circles everywhere
        
        fprintf(fhd1, "\n<defs>\n\n\n\n");
        
        fprintf(fhd1, "<g id=\"substrate layer B with circles everywhere\">\n");
        
        
        fprintf(fhd1, "\n<defs>\n");
        
        fprintf(fhd1, "<g id=\"B layer circle\"  style =\"stroke:rgb(64,64,64); stroke-width:%g; fill:rgb(64,64,64);stroke-opacity:1.0;fill-opacity:1.0;\" > \" \n",STROKE_WIDTH);
        
        fprintf(fhd1,"<circle cx=\"%g\" cy=\"%g\" r=\"%g\" />\n ",0.0,0.0,sphere_radius);
        
        fprintf(fhd1,"</g>\n\n");
        
        fprintf(fhd1, "</defs>\n");
        
        
        for (i=1; i<=NUMBER_OF_ROWS; i++) {
            
            for (j=1; j<=NUMBER_OF_COLUMNS; j++){
                
                x_coordinate=(double)(sphere_radius+((j-1)+(i-1)/2.0)*(sphere_radius*2));
                // the upper line is the shift of each row with index i >1
                
                
                y_coordinate=(double)((sphere_radius+1*((i-1)*sphere_radius*sqrt(3))));
                
                y_coordinate = floor(y_coordinate*10)/10;
                
                
                
                fprintf(fhd1, "<use xlink:href=\"#B layer circle\" x=\"%g\" y=\"%g\"  />\n",x_coordinate,y_coordinate);
                
                
                
            }
            
        }
        
        fprintf(fhd1,"</g>\n</defs>\n");
        
        fprintf(fhd1, "<use xlink:href=\"#substrate layer B with circles everywhere\" transform=\"translate(%g,%g)\" />\n\n",sphere_radius,0.0);
        
    }
    
    
    
    
    // write the substrate layer B with outside circles and a parallelogramm inside
    
    
    if(NUMBER_OF_COLUMNS > UPPER_LIMIT_FOR_SUBSTRATE_DISPLAY) {
        
        
        
        // write the substrate layer B
        
        fprintf(fhd1, "\n<defs> \n");
        
        fprintf(fhd1, "<g id=\"substrate layer B with outside circles\" >\n\n");
        
        fprintf(fhd1, "\n\n\n\n<defs>\n");
        
        fprintf(fhd1, "<g id=\"B layer circle\"  style =\"stroke:rgb(64,64,64); stroke-width:%g; fill:rgb(64,64,64);stroke-opacity:1.0;fill-opacity:1.0;\" > \" \n",STROKE_WIDTH);
        
        fprintf(fhd1,"<circle cx=\"%g\" cy=\"%g\" r=\"%g\" />\n ",0.0,0.0,sphere_radius);
        
        fprintf(fhd1,"</g>\n\n");
        
        fprintf(fhd1, "</defs>");
        
        
        
        for (i=1; i<=NUMBER_OF_ROWS; i++) {
            
            for (j=1; j<=NUMBER_OF_COLUMNS; j++){
                
                x_coordinate=(double)(sphere_radius+((j-1)+(i-1)/2.0)*(sphere_radius*2));
                // the upper line is the shift of each row with index i >1
                
                
                y_coordinate=(double)((sphere_radius+1*((i-1)*sphere_radius*sqrt(3))));
                
                y_coordinate = floor(y_coordinate*10)/10;
                
                
                if (i==1 || i == NUMBER_OF_ROWS || j==1 || j == NUMBER_OF_COLUMNS) {
                    
                    
                    fprintf(fhd1, "<use xlink:href=\"#B layer circle\" x=\"%g\" y=\"%g\"  />\n",x_coordinate,y_coordinate);
                    
                }
                
                
                
                if(i==1 && j==1){
                    
                    top_left_corner_parallelogram_x = x_coordinate;
                    top_left_corner_parallelogram_y = y_coordinate;
                    
                }
                
                
                if(i==1 && j==NUMBER_OF_COLUMNS){
                    
                    top_right_corner_parallelogram_x = x_coordinate;
                    top_right_corner_parallelogram_y = y_coordinate;
                    
                    
                }
                
                if(i==NUMBER_OF_ROWS && j==1){
                    
                    bottom_left_corner_parallelogram_x = x_coordinate;
                    bottom_left_corner_parallelogram_y = y_coordinate;
                    
                }
                
                
                if(i==NUMBER_OF_ROWS && j==NUMBER_OF_COLUMNS){
                    
                    bottom_right_corner_parallelogram_x = x_coordinate;
                    bottom_right_corner_parallelogram_y = y_coordinate;
                    
                }
                
            }
            
        }
        
        
        fprintf(fhd1, "\n<!-- the parallelogramm for layer B -->\n");
        
        fprintf(fhd1, "<polyline points= \"%g %g, %g %g,%g %g,%g %g \" style = \"stroke:rgb(64,64,64); stroke-width:%g; fill:rgb(64,64,64);stroke-opacity:1.0;fill-opacity:1.0;\"  /> ",top_left_corner_parallelogram_x,top_left_corner_parallelogram_y,bottom_left_corner_parallelogram_x,bottom_left_corner_parallelogram_y,bottom_right_corner_parallelogram_x,bottom_right_corner_parallelogram_y,top_right_corner_parallelogram_x,top_right_corner_parallelogram_y,STROKE_WIDTH);
        
        
        
        
        
        
        fprintf(fhd1,"</g>\n</defs>\n");
        
        fprintf(fhd1, "<use xlink:href=\"#substrate layer B with outside circles\" transform=\"translate(%g,%g)\" />\n\n",sphere_radius,0.0);
        
        
        
        
        
        
        
    }
    
    
    
    
    
    
    
    
    
    
    // write the substrate layer C with circles everywhere
    
    if (NUMBER_OF_COLUMNS <= UPPER_LIMIT_FOR_SUBSTRATE_DISPLAY) {
        
        // write the substrate layer C with circles everywhere
        
        fprintf(fhd1, "\n<defs>\n");
        
        fprintf(fhd1,"<g id=\"substrate layer C with circles everywhere\">\n");
        
        
        
        
        
        
        fprintf(fhd1, "\n<defs>\n");
        
        fprintf(fhd1, "<g id=\"C layer circle\"  style =\"stroke:rgb(192,192,192); stroke-width:%g; fill:rgb(192,192,192);stroke-opacity:0.7;fill-opacity:0.7;\" > \" \n",STROKE_WIDTH);
        
        fprintf(fhd1,"<circle cx=\"%g\" cy=\"%g\" r=\"%g\" />\n ",0.0,0.0,sphere_radius);
        
        fprintf(fhd1,"</g>\n");
        
        fprintf(fhd1, "</defs>\n\n\n");
        
        
        
        for (i=1; i<=NUMBER_OF_ROWS; i++) {
            
            for (j=1; j<=NUMBER_OF_COLUMNS; j++){
                
                x_coordinate=(double)(delta_x+sphere_radius+((j-1)+(i-1)/2.0)*(sphere_radius*2));
                
                y_coordinate=(double) (delta_y+1*(sphere_radius+(i-1)*sphere_radius*sqrt(3)));
                
                y_coordinate = floor(y_coordinate*10)/10;
                
                
                fprintf(fhd1, "<use xlink:href=\"#C layer circle\" x=\"%g\" y=\"%g\"  />\n",x_coordinate,y_coordinate);
                
            }
            
        }
        
        //closing the group substrate layer C
        fprintf(fhd1,"</g>\n");
        // closing the the defintion of substrate layer C
        fprintf(fhd1,"\n</defs>\n");
        
        
        fprintf(fhd1, "<use xlink:href=\"#substrate layer C with circles everywhere\" transform=\"translate(%g,%g)\" />\n\n",sphere_radius,0.0);
        
        
    }
    
    
    
    // write the substrate layer C with outside circles and a parallelogramm inside
    
    if(NUMBER_OF_COLUMNS > UPPER_LIMIT_FOR_SUBSTRATE_DISPLAY){
        
        // write the substrate layer C with outside circles and a parallelogram inside
        
        
        fprintf(fhd1, "\n<defs>\n");
        
        fprintf(fhd1,"<g id=\"substrate layer C with outside circles\">\n");
        
        
        fprintf(fhd1, "\n<defs>\n");
        
        fprintf(fhd1, "<g id=\"C layer circle\"  style =\"stroke:rgb(192,192,192); stroke-width:%g; fill:rgb(192,192,192);stroke-opacity:0.7;fill-opacity:0.7;\" > \" \n",STROKE_WIDTH);
        
        fprintf(fhd1,"<circle cx=\"%g\" cy=\"%g\" r=\"%g\" />\n ",0.0,0.0,sphere_radius);
        
        fprintf(fhd1,"</g>\n");
        
        fprintf(fhd1, "</defs>\n\n\n");
        
        
        
        
        
        
        
        
        for (i=1; i<=NUMBER_OF_ROWS; i++) {
            
            for (j=1; j<=NUMBER_OF_COLUMNS; j++){
                
                
                x_coordinate=(double)(delta_x+sphere_radius+((j-1)+(i-1)/2.0)*(sphere_radius*2));
                
                y_coordinate=(double) (delta_y+1*(sphere_radius+(i-1)*sphere_radius*sqrt(3)));
                
                y_coordinate = floor(y_coordinate*10)/10;
                
                
                
                if (i==1 || i == NUMBER_OF_ROWS || j==1 || j == NUMBER_OF_COLUMNS) {
                    
                    
                    
                    fprintf(fhd1, "<use xlink:href=\"#C layer circle\" x=\"%g\" y=\"%g\"  />\n",x_coordinate,y_coordinate);
                    
                    
                    
                }
                
                if(i==1 && j==1){
                    
                    top_left_corner_parallelogram_x = x_coordinate;
                    top_left_corner_parallelogram_y = y_coordinate;
                    
                }
                
                
                if(i==1 && j==NUMBER_OF_COLUMNS){
                    
                    top_right_corner_parallelogram_x = x_coordinate;
                    top_right_corner_parallelogram_y = y_coordinate;
                    
                    
                }
                
                if(i==NUMBER_OF_ROWS && j==1){
                    
                    bottom_left_corner_parallelogram_x = x_coordinate;
                    bottom_left_corner_parallelogram_y = y_coordinate;
                    
                }
                
                
                if(i==NUMBER_OF_ROWS && j==NUMBER_OF_COLUMNS){
                    
                    bottom_right_corner_parallelogram_x = x_coordinate;
                    bottom_right_corner_parallelogram_y = y_coordinate;
                    
                }
                
                
                
                
            }
            
        }
        
        
        
        fprintf(fhd1, "\n\n\n<!-- the parallelogramm for layer C -->\n");
        
        fprintf(fhd1, "<polyline points= \"%g %g, %g %g,%g %g,%g %g \" style = \"stroke:rgb(192,192,192); stroke-width:%g; fill:rgb(192,192,192);stroke-opacity:0.7;fill-opacity:0.7;>\n",top_left_corner_parallelogram_x,top_left_corner_parallelogram_y,bottom_left_corner_parallelogram_x,bottom_left_corner_parallelogram_y,bottom_right_corner_parallelogram_x,bottom_right_corner_parallelogram_y,top_right_corner_parallelogram_x,top_right_corner_parallelogram_y,STROKE_WIDTH);
        fprintf(fhd1,"\n </polyline points>\n");
        
        
        
        //closing the group substrate layer C
        fprintf(fhd1,"</g>\n");
        // closing the the defintion of substrate layer C
        fprintf(fhd1,"\n</defs>\n");
        
        
        fprintf(fhd1, "<use xlink:href=\"#substrate layer C with outside circles\" transform=\"translate(%g,%g)\" />\n\n",+sphere_radius,0.0);
        
        
        
        
    }
    
    
    
    
    // Prepare the adsorbate layer
    
    fprintf(fhd1, "\n<defs>\n");
    
    fprintf(fhd1, "<g id=\"adsorbate layer animated\">\n");
    
    
    fprintf(fhd1, "\n<defs>\n");
    
    fprintf(fhd1, "<g id=\"adsorbate layer circle\"  style =\"stroke:rgb(255,255,0); stroke-width:%g; fill:rgb(255,255,0);stroke-opacity:1.0;fill-opacity:1.0;\" > \" \n",STROKE_WIDTH);
    
    fprintf(fhd1,"<circle cx=\"%g\" cy=\"%g\" r=\"%g\" />\n ",0.0,0.0,sphere_radius);
    
    fprintf(fhd1,"</g>\n\n");
    
    fprintf(fhd1, "</defs>\n");
    
    
    
    
    
    
    // Now we write the animation for the adsorbate layer
    
    
    for (index_in_history_adsorbate_positions=1; index_in_history_adsorbate_positions<= mcs.lines_in_history_of_adsorbates_positions; index_in_history_adsorbate_positions++) {
        
        for(index_timed_output=1; index_timed_output<mcs.lines_in_experiment_monitor_file;index_timed_output++)
            
            //  index_running_through_adsorbates=0;
            
            if (mcs.history_of_adsorbate_positions[index_in_history_adsorbate_positions][1]==index_timed_output) {
                
                if (mcs.history_of_adsorbate_positions[index_in_history_adsorbate_positions][3]==1) {
                    
                    fprintf(fhd1, "<g id=\"adsorbates at %i\" style =\"visibility:hidden;\" >\n",index_timed_output);
                    
                }
                
                i_coor=mcs.history_of_adsorbate_positions[index_in_history_adsorbate_positions][4];
                
                j_coor=mcs.history_of_adsorbate_positions[index_in_history_adsorbate_positions][5];
                
                x_coordinate=(double)(2*delta_x+sphere_radius+((j_coor-1)+(i_coor-1)/2.0)*(sphere_radius*2));
                
                y_coordinate=(double) (2*delta_y+1*(sphere_radius+1*(i_coor-1)*sphere_radius*sqrt(3)));
                
                y_coordinate = floor(y_coordinate*10)/10;
                
                
                fprintf(fhd1, "<use xlink:href=\"#adsorbate layer circle\" x=\"%g\" y=\"%g\"  />\n",x_coordinate,y_coordinate);
                
                // now we ask if the the ad_id is the total number of adsorbates
                
                if (mcs.history_of_adsorbate_positions[index_in_history_adsorbate_positions][3]== mcs.history_of_adsorbate_positions[index_in_history_adsorbate_positions][2]) {
                    
                    
                    // now we must finish this time step
                    
                    
                    
                    fprintf(fhd1,"\n <text x=\"%g\" y=\"%g\" style=\"font-size: %ipt; font-weight:bold; font-family:sans-serif;\">\n",line_spacing/2.0-sphere_radius,page_height-1.0*line_spacing,FONT_SIZE_LEGEND);
                    
                    fprintf(fhd1,"\nNumber of adsorbates:   %3i\n", mcs.history_of_adsorbate_positions[index_in_history_adsorbate_positions][2]);
                    
                    fprintf(fhd1,"</text>\n");
                    
                    
                    
                    fprintf(fhd1,"\n <text x=\"%g\" y=\"%g\" style=\"font-size: %ipt; font-weight:bold; font-family:sans-serif;\">\n",line_spacing/2.0-sphere_radius,page_height-2.0*line_spacing,FONT_SIZE_LEGEND);
                    
                    fprintf(fhd1,"\nTemperature:   %3i K\n", mcs.history_of_adsorbate_positions[index_in_history_adsorbate_positions][6]);
                    
                    fprintf(fhd1,"</text>\n");
                    
                    
                    fprintf(fhd1,"\n <text x=\"%g\" y=\"%g\" style=\"font-size: %ipt; font-weight:bold; font-family:sans-serif;\">\n",line_spacing/2.0-sphere_radius,page_height-3.0*line_spacing,FONT_SIZE_LEGEND);
                    
                    fprintf(fhd1,"\nTime:   %6.3g sec\n",(double)(((mcs.history_of_adsorbate_positions[index_in_history_adsorbate_positions][1]-1)*1.0)/FRAME_RATE));
                    
                    fprintf(fhd1,"</text>\n");
                    
                    
                    
                    
                    
                    
                    if(index_in_history_adsorbate_positions<mcs.lines_in_history_of_adsorbates_positions)
                        // remove this animation after duration time
                    {
                        fprintf(fhd1,"<set attributeName=\"visibility\" attributeType=\"CSS\" to=\"visible\" begin=\"%gs\" dur=\"%gs\" fill=\"remove\"/>\n",(double)(index_timed_output)/((double)(FRAME_RATE))*DURATION_FACTOR_ANIMATED_SVG,1.0/((double)(FRAME_RATE))*(double)(DURATION_FACTOR_ANIMATED_SVG) );
                    }
                    
                    // The difference between the two elements is the freeze and remove
                    if(index_in_history_adsorbate_positions==mcs.lines_in_history_of_adsorbates_positions)
                    {
                        // freeze this animation after duration time
                        
                        fprintf(fhd1,"<set attributeName=\"visibility\" attributeType=\"CSS\" to=\"visible\" begin=\"%gs\" dur=\"%gs\" fill=\"freeze\"/>\n",(double)(index_timed_output)/((double)(FRAME_RATE))*DURATION_FACTOR_ANIMATED_SVG,1.0/((double)(FRAME_RATE))*(double)(DURATION_FACTOR_ANIMATED_SVG) );
                    }
                    
                    // the difference between remove and freeze
                    
                    
                    
                    fprintf(fhd1,"</g>\n");
                    
                }
                
                
                
            }
        
        
        
    }
    
    
    
    fprintf(fhd1,"</g>\n");
    
    //
    
    /*
     fprintf(fhd1,"\n<text  style=\"font-size: %ipt; font-weight:bold; font-family:sans-serif;\"> ",FONT_SIZE_LEGEND);
     
     fprintf(fhd1,"\n<text x=\"%g\" y=\"%g\">",line_spacing/2.0,page_height-4.0*line_spacing);
     
     fprintf(fhd1,"\n    Layer B");
     
     fprintf(fhd1,"\n</text>");
     */
    //
    
    
    fprintf(fhd1,"</defs>\n\n");
    
    fprintf(fhd1, "<use xlink:href=\"#adsorbate layer animated\" transform=\"translate(%g,%g)\" />\n\n",sphere_radius,0.0);
    
    
    
    
    
    
    
    
    
    
    
    
    
    // Now the legend
    
    
    // the circles for the legend
    
    fprintf(fhd1, "\n\n\n\n<defs>\n");
    
    
    fprintf(fhd1, "<g id=\"B layer circle for legend\"  style =\"stroke:rgb(64,64,64); stroke-width:%g; fill:rgb(64,64,64);stroke-opacity:1.0;fill-opacity:1.0;\" > \" \n",STROKE_WIDTH);
    
    fprintf(fhd1,"<circle cx=\"%g\" cy=\"%g\" r=\"%g\" />\n ",0.0,0.0,(double)(CIRCLE_RADIUS_LEGEND));
    
    
    fprintf(fhd1,"</g>\n\n");
    
    fprintf(fhd1, "<g id=\"C layer circle for legend\"  style =\"stroke:rgb(192,192,192); stroke-width:%g; fill:rgb(192,192,192);stroke-opacity:0.7;fill-opacity:0.7;\" > \" \n",STROKE_WIDTH);
    
    fprintf(fhd1,"<circle cx=\"%g\" cy=\"%g\" r=\"%g\" />\n ",0.0,0.0,(double)(CIRCLE_RADIUS_LEGEND));
    
    
    fprintf(fhd1,"</g>\n");
    
    fprintf(fhd1, "<g id=\"adsorbate layer circle for legend\"  style =\"stroke:rgb(255,255,0); stroke-width:%g; fill:rgb(255,255,0);stroke-opacity:1.0;fill-opacity:1.0;\" > \" \n",STROKE_WIDTH);
    
    fprintf(fhd1,"<circle cx=\"%g\" cy=\"%g\" r=\"%g\" />\n ",0.0,0.0,(double)(CIRCLE_RADIUS_LEGEND) );
    
    fprintf(fhd1,"</g>\n\n");
    
    
    fprintf(fhd1, "</defs>");
    
    
    
    
    // The legend
    
    
    fprintf(fhd1,"\n<g style=\"font-size: %ipt; font-weight:bold; font-family:sans-serif;\"> ",FONT_SIZE_LEGEND);
    
    fprintf(fhd1,"\n<text x=\"%g\" y=\"%g\">",line_spacing/2.0,page_height-6.0*line_spacing);
    
    fprintf(fhd1,"\n    Layer B");
    
    fprintf(fhd1,"\n</text>");
    
    fprintf(fhd1,"\n<text x=\"%g\" y=\"%g\">",line_spacing/2.0,page_height-5.0*line_spacing);
    
    fprintf(fhd1,"\n    Layer C");
    
    fprintf(fhd1,"\n</text>");
    
    fprintf(fhd1,"\n<text x=\"%g\" y=\"%g\">",line_spacing/2.0,page_height-4.0*line_spacing);
    
    fprintf(fhd1,"\n    Adsorbate");
    
    fprintf(fhd1,"\n</text>");
    
    
    
    
    
    fprintf(fhd1, "</g>    ");
    
    
    
    
    
    fprintf(fhd1, "<use xlink:href=\"#B layer circle for legend\" x=\"%g\" y=\"%g\" />\n",line_spacing/2.0+45.0,page_height-6.0*line_spacing-2.5);
    
    fprintf(fhd1, "<use xlink:href=\"#C layer circle for legend\" x=\"%g\" y=\"%g\" />\n",line_spacing/2.0+45.0,page_height-5.0*line_spacing-2.5);
    
    fprintf(fhd1, "<use xlink:href=\"#adsorbate layer circle for legend\" x=\"%g\" y=\"%g\" />\n",line_spacing/2.0+45.0,page_height-4.0*line_spacing-2.5);
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    fprintf(fhd1, "\n</svg>\n\n");
    
    fclose(fhd1);
    
    
    
    
}



void data_output_eps_ad_id(struct MCS_STRUCT *ptr_to_mcs_struct){
    
    // this procedure writes a file of  the adsorbate structure in encapsulated postscript format
    // the adsorbate atoms are tagged with the adsorbate id, the substrate atoms are tagged with the i and j coordinate when UPPER_LIMIT_FOR_SUBSTRATE_DISPLAY > NUMBER_OF_COLUMNS
    // this procedure is only called when the variable DIAGNOSTICS_TOOLS in line 38 is set to true.
    
    ptr_to_mcs_struct=&mcs;
    
    FILE *fhd1;
    
    int i,j,k;
    
    int ad_id=0;
    
    int temperature_growth=TEMPERATURE_DURING_GROWTH;
    
    char local_filename[200]="";
    
    char number[200];//,command[200]="";
    
    double x_coordinate=0.0,y_coordinate=0.0,delta_x=0.0,delta_y=0.0;
    
    int font_size_legend=9;
    
    double line_spacing=1.5*font_size_legend;
    
    //  a double column figure in a journal typically has a width of 16 cm
    //  this means 16/2.54 inches
    //  Postscript default resolution 72 points pro inch
    
    double final_page_width=16./2.54*72;;
    
    double tentative_page_height=final_page_width/(sqrt(2.));
    
    double relative_sphere_radius=1.0;
    
    
    double sphere_radius=tentative_page_height/(2.*relative_sphere_radius+NUMBER_OF_ROWS*sqrt(3)*relative_sphere_radius);
    
    sphere_radius = floor(sphere_radius*10)/10;
    
    double y_value_lowest_row=font_size_legend*1;
    
    
    double test_bottom_right_y=sphere_radius*(2.-sqrt(3)/3.+sqrt(3)*NUMBER_OF_ROWS);
    
    double test_bottom_right_x=4*sphere_radius+(NUMBER_OF_COLUMNS+NUMBER_OF_ROWS/2.-3./2.)*sphere_radius*2;
    
    double measured_page_height=test_bottom_right_y;
    
    double measured_page_width=test_bottom_right_x;
    
    double correctur_factor=measured_page_width/measured_page_height;
    
    double page_width=final_page_width;
    
    double page_height=page_width/correctur_factor;
    
    // recalculation of the sphere radius
    
    
    sphere_radius=page_height/(2.*relative_sphere_radius+NUMBER_OF_ROWS*sqrt(3)*relative_sphere_radius);
    
    sphere_radius = floor(sphere_radius*10)/10;
    
    // Layer C is shifted towards layer B //
    
    // The shift in x-direction is called delta_x , its value is sphere_radius
    // The shift in x-direction is called delta_y , its value is minus sphere_radius times sqrt(3)/3
    // the later comes from the geometrical calculation that the layer C atom is just in the middle of
    // an equilateral triangle which is spanned by three atoms of the layer B (the layer below C)
    
    delta_x=sphere_radius;
    
    delta_y=-sphere_radius*tan(M_PI/6);
    
    
    
    // in case of number of columns and rows larger than for example 150 do not write the layers B and C but a simple gray parallelogramm instead
    
    
    
    
    // define the parallelogramm which replaces layer c for greater matrices
    
    
    
    
    // seen from the top left corner of the page
    
    double
    
    top_left_corner_parallelogram_x=2*sphere_radius ,
    top_left_corner_parallelogram_y=-1.*sphere_radius*(1.+sqrt(3)/3.),
    bottom_left_corner_parallelogram_x=sphere_radius+sphere_radius*NUMBER_OF_ROWS,
    bottom_left_corner_parallelogram_y=(.1547005390*sphere_radius-1.732050808*sphere_radius*NUMBER_OF_ROWS),
    top_right_corner_parallelogram_x=2*sphere_radius*NUMBER_OF_COLUMNS,
    top_right_corner_parallelogram_y= top_left_corner_parallelogram_y, // ist ungeaendert
    bottom_right_corner_parallelogram_x=-1.*sphere_radius+2.*sphere_radius*NUMBER_OF_COLUMNS+sphere_radius*NUMBER_OF_ROWS,
    bottom_right_corner_parallelogram_y=(.1547005390*sphere_radius-1.732050808*sphere_radius*NUMBER_OF_ROWS);
    
    
    
    // The layers B, C and the adsorbate layer are shifted vs each other
    // by sphere_radius in x and by sphere_radius*tan(30 deg) in y
    
    int seconds_experiment=0,milli_seconds_experiment=0;
    
    
    if (mcs.mc_time<1) {
        
        milli_seconds_experiment=(int) (trunc(mcs.mc_time*1000)) ;
        
        
        sprintf(number,"steps_%09ld_T_initial_%03iK_T_actual_%03iK_MC_time_%03ims_%04i_adsorbates",mcs.number_mc_step,temperature_growth,(int)(mcs.temperature),milli_seconds_experiment,mcs.number_of_adsorbates);
        
        
        
    }
    
    
    if (mcs.mc_time>1 && mcs.mc_time<mcs.duration_of_experiment) {
        
        
        seconds_experiment=(int) (trunc(mcs.mc_time)) ;
        
        milli_seconds_experiment=(int) (trunc(mcs.mc_time*1000-seconds_experiment*1000)) ;
        
        sprintf(number,"steps_%09ld_T_initial_%03iK_T_actual_%03iK_MC_time_%02is_%03ims_%04i_adsorbates",mcs.number_mc_step,temperature_growth,(int)(mcs.temperature),seconds_experiment,milli_seconds_experiment,mcs.number_of_adsorbates);
        
        
        
    }
    
    
    
    
    
    
    
    
    strcat(local_filename,DATA_DIRECTORY);
    
    
    strcat(local_filename,"adsorb_struc_");
    
    
    
    strcat(local_filename,number);
    
    strcat(local_filename,".eps");
    
    
    
    fhd1=fopen(local_filename,"w+");
    
    
    if (fhd1==NULL) {
        
        printf("\n\n Warning \n\n The file named \n -->%s<-- \ncould not be opened due to an error\nPlease check the line 100 of the  source code\n   ",local_filename);
        
        
    }
    
    
    else {
        
        printf("\n\nA file with the name \n-->%s<--\nwas successfully opened \n  ",local_filename);
        
        
    }
    
    
    
    
    fprintf(fhd1,"%%!PS-Adobe-2.0 EPSF-2.0\n");
    fprintf(fhd1,"%%");fprintf(fhd1,"%%");
    fprintf(fhd1,"BoundingBox: 0 0 %d  %d \n", (int)(ceil(page_width)) ,(int)(ceil(page_height)) );
    fprintf(fhd1,"%%");fprintf(fhd1,"%%");
    fprintf(fhd1,"Creator: University of Duisburg-Essen\n");
    fprintf(fhd1,"%%");fprintf(fhd1,"%%");
    fprintf(fhd1,"For: American Chemical Society\n");
    fprintf(fhd1,"%%");fprintf(fhd1,"%%");
    fprintf(fhd1,"EndComments\n");
    
    fprintf(fhd1,"%g %g translate \n",0.,page_height);
    
    
    
    
    
    if (NUMBER_OF_COLUMNS<UPPER_LIMIT_FOR_SUBSTRATE_DISPLAY) {
        
        // the following double for loop writes the substrate layer B
        
        // this layer is not shifted
        
        fprintf(fhd1,"0.0 0.0 0.0 0.75 setcmykcolor\n");
        
        for (i=1; i<=NUMBER_OF_ROWS; i++) {
            
            for (j=1; j<=NUMBER_OF_COLUMNS; j++){
                
                x_coordinate=(double)(sphere_radius+((j-1)+(i-1)/2.0)*(sphere_radius*2));
                // the upper line is the shift of each row with index i >1
                
                
                y_coordinate=(double)(-1*(sphere_radius+1*((i-1)*sphere_radius*sqrt(3))));
                
                
                y_coordinate = floor(y_coordinate*10)/10;
                
                
                
                fprintf(fhd1," %g %g moveto \n",x_coordinate,y_coordinate);
                fprintf(fhd1,"%g %g %g %g %g arc \n",x_coordinate,y_coordinate,sphere_radius,0.,360.);
                
                
                
            };
            
            fprintf(fhd1, "closepath fill\n");
        }
        
    }
    
    // the following double for loop writes the substrate layer C
    
    // Layer C is shifted towards the previously written layer B
    
    
    fprintf(fhd1,"0.0 0.0 0.0 0.5 setcmykcolor\n");
    
    
    for (i=1; i<=NUMBER_OF_ROWS; i++) {
        
        for (j=1; j<=NUMBER_OF_COLUMNS; j++){
            
            
            if (NUMBER_OF_COLUMNS<UPPER_LIMIT_FOR_SUBSTRATE_DISPLAY) {
                
                
                x_coordinate=(double)(delta_x+sphere_radius+((j-1)+(i-1)/2.0)*(sphere_radius*2));
                
                y_coordinate=(double) (delta_y-1*(sphere_radius+(i-1)*sphere_radius*sqrt(3)));
                
                
                y_coordinate = floor(y_coordinate*10)/10;
                
                
                
                fprintf(fhd1," %g %g moveto \n",x_coordinate,y_coordinate);
                
                fprintf(fhd1,"%g %g %g %i %i arc \n",x_coordinate,y_coordinate,sphere_radius,0,360);
                
                fprintf(fhd1,"closepath fill\n");
                
                
            }
            
            
            else {
                
                
                if (i==1 || i == NUMBER_OF_COLUMNS || j==1 || j== NUMBER_OF_ROWS) {
                    
                    x_coordinate=(double)(delta_x+sphere_radius+((j-1)+(i-1)/2.0)*(sphere_radius*2));
                    
                    y_coordinate=(double) (delta_y-1*(sphere_radius+(i-1)*sphere_radius*sqrt(3)));
                    
                    y_coordinate = floor(y_coordinate*10)/10;
                    
                    
                    
                    fprintf(fhd1,"%g %g moveto \n",x_coordinate,y_coordinate);
                    
                    fprintf(fhd1,"%g %g %g %i %i arc \n",x_coordinate,y_coordinate,sphere_radius,0,360);
                    
                    fprintf(fhd1,"closepath fill \n ");
                    
                    
                }
                
                
            }
            
            
            
        }
        
        
        
    }
    
    
    
    
    if (NUMBER_OF_COLUMNS > UPPER_LIMIT_FOR_SUBSTRATE_DISPLAY) {
        
        
        fprintf(fhd1, "%%  Now the parallelogramm representing the innner atoms is written \n");
        
        fprintf(fhd1," 0.0 0.0 0.0 0.5 setcmykcolor\n");
        
        fprintf(fhd1,"%g %g moveto \n ",top_left_corner_parallelogram_x,top_left_corner_parallelogram_y);
        fprintf(fhd1,"%g %g lineto \n ", bottom_left_corner_parallelogram_x,bottom_left_corner_parallelogram_y);
        fprintf(fhd1,"%g %g lineto \n ", bottom_right_corner_parallelogram_x,bottom_right_corner_parallelogram_y);
        fprintf(fhd1,"%g %g lineto \n ", top_right_corner_parallelogram_x,top_right_corner_parallelogram_y);
        
        fprintf(fhd1," closepath fill \n ");
        
        
        
    }
    
    
    
    
    
    // The following double for loop writes the adsorbate matrix
    
    // The adsorbate lattice has to be shifted versus substrate layer C
    
    
    
    fprintf(fhd1, "%% Now the adsorbate layer is going to be  written\n ");
    
    
    fprintf(fhd1,"0.0 0.0 1.0 0.0 setcmykcolor\n");
    
    
    //  fprintf(fhd1,"%g %g translate \n",0.,page_height);
    
    fprintf(fhd1,"/Helvetica %g selectfont\n",0.55);
    
    
    for (i=1; i<=NUMBER_OF_ROWS; i++) {
        
        for (j=1; j<=NUMBER_OF_COLUMNS; j++){
            
            if(free_occupied_detector(i, j, &mcs)==1){
                
                for (k=1; k<=mcs.number_of_adsorbates*6; k++) {
                    
                    if (mcs.table_of_adsorbate_data[k][2]==i && mcs.table_of_adsorbate_data[k][3]==j  ) {
                        ad_id=mcs.table_of_adsorbate_data[k][1];
                    }
                    
                    
                }
                
                
                x_coordinate=(double)(2*delta_x+sphere_radius+((j-1)+(i-1)/2.0)*(sphere_radius*2));
                // the upper line is the shift of each row with index i >1
                
                
                y_coordinate=(double) (2*delta_y-1*(sphere_radius+1*((i-1)*sphere_radius*2*sqrt(3)/2.0)));
                
                fprintf(fhd1,"%g %g moveto \n",x_coordinate,y_coordinate);
                
                fprintf(fhd1,"%g %g %g %i %i arc \n",x_coordinate,y_coordinate,sphere_radius,0,360);
                fprintf(fhd1, "closepath fill\n ");
                
                fprintf(fhd1,"%g %g moveto \n",x_coordinate-0.5,y_coordinate-0.15);
                
                fprintf(fhd1, "0.0 0.0 0.0 1.0 setcmykcolor\n");
                
                fprintf(fhd1,"(%03i) show \n",ad_id);
                
                fprintf(fhd1,"0.0 0.0 1.0 0.0 setcmykcolor\n");
                
                
                
                
            }
            
        }
        
        
    }
    
    
    
    
    fprintf(fhd1,"%g %g translate ",0., -page_height);
    
    
    
    
    
    
    if (NUMBER_OF_COLUMNS<UPPER_LIMIT_FOR_SUBSTRATE_DISPLAY) {
        
        
        fprintf(fhd1," 0.0 0.0 0.0 0.75 setcmykcolor\n");
        
        fprintf(fhd1,"%g %g moveto \n",font_size_legend+1.5,y_value_lowest_row+5*line_spacing);
        
        fprintf(fhd1,"%g %g %g 0 360 arc closepath fill\n ",font_size_legend+1.5,y_value_lowest_row+5*line_spacing,font_size_legend/2.);
        
        fprintf(fhd1,"0.0 0.0 0.0 1.0  setcmykcolor\n");
        
        fprintf(fhd1,"/Helvetica %i selectfont\n",font_size_legend);
        
        fprintf(fhd1,"%g %g moveto (Layer B) show \n",1.5+font_size_legend*2.0,y_value_lowest_row-font_size_legend/2.5+5*line_spacing);
        
    }
    
    
    
    
    
    
    
    
    fprintf(fhd1," 0.0 0.0 0.0 0.5 setcmykcolor\n");
    
    fprintf(fhd1,"%g %g moveto \n",font_size_legend+1.5,y_value_lowest_row+4*line_spacing );
    
    fprintf(fhd1,"%g %g %g 0 360 arc\nclosepath fill\n ",font_size_legend+1.5,y_value_lowest_row+4*line_spacing,font_size_legend/2.);
    
    fprintf(fhd1,"0.0 0.0 0.0 1.0 setcmykcolor\n" );
    
    fprintf(fhd1,"/Helvetica %i selectfont\n",font_size_legend);
    
    fprintf(fhd1,"%g %g moveto \n (Layer C) show \n",1.5+font_size_legend*2.0,y_value_lowest_row-font_size_legend/2.5+4*line_spacing);
    
    
    
    
    
    
    
    
    fprintf(fhd1,"0.0 0.0 1.0 0.0 setcmykcolor\n");
    
    fprintf(fhd1,"%g %g moveto \n",font_size_legend+1.5,y_value_lowest_row+3*line_spacing );
    
    fprintf(fhd1,"%g %g %g 0 360 arc\nclosepath fill\n ",font_size_legend+1.5,y_value_lowest_row+3*line_spacing,font_size_legend/2.);
    
    fprintf(fhd1,"0.0 0.0 0.0 1.0 setcmykcolor\n");
    
    fprintf(fhd1,"/Helvetica %i selectfont\n",font_size_legend);
    
    
    fprintf(fhd1,"%g %g moveto \n (Adsorbate layer) show \n",1.5+2*font_size_legend,y_value_lowest_row-font_size_legend/2.5+3*line_spacing);
    
    
    
    
    
    
    
    if (mcs.mc_time<1) {
        fprintf(fhd1,"%g %g moveto \n (Time: %d ms) show \n",font_size_legend*0.5+1.5,y_value_lowest_row-font_size_legend/2.5+2*line_spacing,(int)(mcs.mc_time*1000));
    }
    
    else{
        fprintf(fhd1,"%g %g moveto \n (Time: %g s) show \n",font_size_legend*0.5+1.5,y_value_lowest_row-font_size_legend/2.5+2*line_spacing,(double)((int)(mcs.mc_time*100)/100.));
    }
    
    
    fprintf(fhd1,"%g %g moveto \n (Growth temperature: %d K) show \n",font_size_legend*0.5+1.5,y_value_lowest_row-font_size_legend/2.5+1*line_spacing,TEMPERATURE_DURING_GROWTH);
    
    fprintf(fhd1,"%g %g moveto \n (Actual temperature: %d K) show \n",font_size_legend*0.5+1.5,y_value_lowest_row-font_size_legend/2.5,(int)(mcs.temperature));
    
    
    fprintf(fhd1,"\n");
    
    fclose(fhd1);
    
    
    
    
}




void data_output_experiment_monitor_file(struct MCS_STRUCT *ptr_to_mcs_struct)
{
    
    // this procedure writes the table mcs.experiment_monitor to a csv file
    // this procedure is always called together with the data_outpt_eps routine
    
    ptr_to_mcs_struct=&mcs;
    FILE *fhd1;
    int i;
    
    clock_t t_actual_time=clock();
    double cpu_time_used=(t_actual_time-mcs.start_clock_of_mcs)*1./CLOCKS_PER_SEC;
    
    
    int new_number_of_entries=mcs.lines_in_experiment_monitor_file+1;
    
    
    // get the old data
    
    mcs.experiment_monitor=(double **) realloc(mcs.experiment_monitor,(new_number_of_entries+1)*sizeof(double *) );
    
    // the point of the new row must be initialised to NULL
    
    mcs.experiment_monitor[new_number_of_entries]=NULL;
    
    //reallocate the rows
    
    for (i=1; i<=new_number_of_entries; i++) {
        
        mcs.experiment_monitor[i]=(double *)realloc(mcs.experiment_monitor[i],(NUMBER_OF_COLUMNS_IN_EXPERIMENT_MONITOR_FILE+1)*sizeof(double) )  ;
    }
    
    
    
    
    
    mcs.experiment_monitor[new_number_of_entries][1]=mcs.mc_time;
    mcs.experiment_monitor[new_number_of_entries][2]=mcs.temperature;
    mcs.experiment_monitor[new_number_of_entries][3]=mcs.number_of_adsorbates;
    mcs.experiment_monitor[new_number_of_entries][4]=mcs.number_of_adsorbates*1./(NUMBER_OF_COLUMNS*NUMBER_OF_ROWS); // the coverage
    mcs.experiment_monitor[new_number_of_entries][5]=mcs.number_mc_step;
    mcs.experiment_monitor[new_number_of_entries][6]=cpu_time_used;
    mcs.experiment_monitor[new_number_of_entries][7]=mcs.process_counter[8]; // rewrites of neighbours and single rates
    mcs.experiment_monitor[new_number_of_entries][8]=mcs.process_counter[9];  // sorting of the table
    mcs.experiment_monitor[new_number_of_entries][9]=mcs.process_counter[1];  // diffusion direction 1 counter
    mcs.experiment_monitor[new_number_of_entries][10]=mcs.process_counter[2];  // diffusion direction 2 counter
    mcs.experiment_monitor[new_number_of_entries][11]=mcs.process_counter[3];  // diffusion direction 3 counter
    mcs.experiment_monitor[new_number_of_entries][12]=mcs.process_counter[4];  // diffusion direction 4 counter
    mcs.experiment_monitor[new_number_of_entries][13]=mcs.process_counter[5];  // diffusion direction 5 counter
    mcs.experiment_monitor[new_number_of_entries][14]=mcs.process_counter[6];  // diffusion direction 6 counter
    mcs.experiment_monitor[new_number_of_entries][15]= (double) (  mcs.number_of_adsorbates - mcs.number_of_adsorbates_with_less_than_six_neigh ) ;  // number of enclosed atoms
    
    
    
    
    
    
    fhd1=fopen(mcs.fname_experiment_monitor,"w+");
    
    if (fhd1==NULL) {
        
        printf("\n\n Warning \n\n The file named \n -->%s<-- \ncould not be open due to an error\nPlease check the line 100 of the  source code\n   ",mcs.fname_experiment_monitor);
        
        
    }
    
    
    else {
        
        printf("\n\n The experiment monitor file with the name \n-->%s<--\nwas successfully opened \n  ",mcs.fname_experiment_monitor);
        
        
    }
    
    
    
    
    
    
    
    
    
    fprintf(fhd1," file_number,mc_time, temperature, number_adsorbates, coverage, mc_step, cpu_time, rewrites, sorting, diff1, diff2, diff3, diff4, diff5, diff6, enclosed_atoms \n");
    
    
    for (i=1; i<=new_number_of_entries; i++) {
        
        fprintf(fhd1, " %03d,%g,\t%g,\t%g,\t%g,\t%g,\t%g,\t%d,\t%d,\t%d,\t%d,\t%d,\t%d,\t%d,\t%d,\t%g \n",i,mcs.experiment_monitor[i][1],mcs.experiment_monitor[i][2],mcs.experiment_monitor[i][3],mcs.experiment_monitor[i][4],mcs.experiment_monitor[i][5],mcs.experiment_monitor[i][6],(int)(mcs.experiment_monitor[i][7]),(int)(mcs.experiment_monitor[i][8]),(int)(mcs.experiment_monitor[i][9]),(int)(mcs.experiment_monitor[i][10]),(int)(mcs.experiment_monitor[i][11]),(int)(mcs.experiment_monitor[i][12]),(int)(mcs.experiment_monitor[i][13]),(int)(mcs.experiment_monitor[i][14] ),mcs.experiment_monitor[i][15] );
        
    }
    
    
    fprintf(fhd1,"\n");
    
    fclose(fhd1);
    
    mcs.lines_in_experiment_monitor_file=new_number_of_entries;
    
    
}


void rewrite_history_of_adsorbates(struct MCS_STRUCT *ptr_to_mcs_struct){
    
    // this procedure rewrites the history of adsorbates
    
    ptr_to_mcs_struct=&mcs;
    
    int i=0,ad_id=0;
    
    int former_number_of_entries_in_history=mcs.lines_in_history_of_adsorbates_positions;
    
    int new_number_of_entries_in_history=former_number_of_entries_in_history+mcs.number_of_adsorbates;
    
    mcs.lines_in_history_of_adsorbates_positions=new_number_of_entries_in_history;
    
    // get the old data
    
    mcs.history_of_adsorbate_positions=(int **) realloc(mcs.history_of_adsorbate_positions, (new_number_of_entries_in_history+1)*sizeof(int *));
    
    // the pointer to the new rows must be initialized to NULL
    
    for (i=former_number_of_entries_in_history+1; i<=new_number_of_entries_in_history; i++) {
        mcs.history_of_adsorbate_positions[i]=NULL;
        
    }
    
    // reallocate all the rows
    
    for (i=1; i<=new_number_of_entries_in_history; i++) {
        mcs.history_of_adsorbate_positions[i]= (int *) realloc(mcs.history_of_adsorbate_positions[i],(NUMBER_OF_COLUMNS_IN_HISTORY_OF_ADSORBATE_POSITIONS+1)*sizeof(int));
    }
    
    
    // now the data for the new index to the history of adsorbate data
    
    
    
    ad_id=0;
    
    
    for (i=former_number_of_entries_in_history+1; i<=new_number_of_entries_in_history; i++) {
        
        ad_id=ad_id+1; 	// j is the index for the adsorbates at this mcs.line_in_experiment_monitor_file;
        
        //  the line below means if the second SVG or eps file of the adsorbate structure is written, mcs.line_in_experiment_monitor_file is 2
        
        mcs.history_of_adsorbate_positions[i][1]=mcs.line_in_experiment_monitor_file;  // sometimes also called the index
        
        mcs.history_of_adsorbate_positions[i][2]=mcs.number_of_adsorbates;  // the total number of adsorbates at this time or index
        
        mcs.history_of_adsorbate_positions[i][3]=ad_id;
        
        mcs.history_of_adsorbate_positions[i][4]=mcs.table_of_adsorbate_data[ad_id*6][2];   // the i_coordinate of the adsorbate with the id ad_id
        
        mcs.history_of_adsorbate_positions[i][5]=mcs.table_of_adsorbate_data[ad_id*6][3];   // the j_coordinate of the adsorbate with the id ad_id
        
        mcs.history_of_adsorbate_positions[i][6]=mcs.temperature;
        
        
        
        
        
        
    }
    
    
    
    
    
    
}



void perform_adsorption(struct MCS_STRUCT *ptr_to_mcs_struct)
{
    
    // this procecdures tries to set an adsorbate atom onto the surface
    
    ptr_to_mcs_struct=&mcs;
    
    
    
    
    int random_i_coordinate=0,random_j_coordinate=0,i_coordinate_adsorbed_atom=0,j_coordinate_adsorbed_atom=0;
    
    bool found_empty_site_for_adsorption=false;
    
    long int i=0;
    
    
    
    
    // find i and j coordinates for a new atom
    
    for (i=1; i<=NUMBER_OF_ROWS*NUMBER_OF_COLUMNS; i++) {
        
        random_i_coordinate=random_integer(NUMBER_OF_ROWS);
        random_j_coordinate=random_integer(NUMBER_OF_COLUMNS);
        
        
        
        
        if(free_occupied_detector(random_i_coordinate, random_j_coordinate,&mcs)==0)
            
        {
            
            
            
            
            mcs.number_of_adsorbates=mcs.number_of_adsorbates+1;
            
            found_empty_site_for_adsorption=true;
            
            mcs.id_of_touched_adsorbate=mcs.number_of_adsorbates;
            
            
            break;
            
        }
        
        
    }
    
    
    i_coordinate_adsorbed_atom=random_i_coordinate;
    
    j_coordinate_adsorbed_atom=random_j_coordinate;
    
    
    if (found_empty_site_for_adsorption==true) {
        
        // the value mcs.number_of_adsorbates corresponds to the id of the adsorbed atom
        
        // append the data for the adsorbed atom
        
        for (i=1; i<=6; i++) {
            
            
            
            
            
            mcs.table_of_adsorbate_data[(mcs.number_of_adsorbates-1)*6+i][1]=(double)(mcs.id_of_touched_adsorbate);
            
            mcs.table_of_adsorbate_data[(mcs.number_of_adsorbates-1)*6+i][2]=(double)(i_coordinate_adsorbed_atom);
            
            mcs.table_of_adsorbate_data[(mcs.number_of_adsorbates-1)*6+i][3]=(double)(j_coordinate_adsorbed_atom);
            
            // the three types of neighbours
            
            mcs.table_of_adsorbate_data[(mcs.number_of_adsorbates-1)*6+i][4]=(double)(0);
            
            mcs.table_of_adsorbate_data[(mcs.number_of_adsorbates-1)*6+i][5]=(double)(0);
            
            mcs.table_of_adsorbate_data[(mcs.number_of_adsorbates-1)*6+i][6]=(double)(0);
            
            // the diffusion direction
            
            mcs.table_of_adsorbate_data[(mcs.number_of_adsorbates-1)*6+i][7]=(double)(i);
            
            // the diffusion rates
            // at first set to zero
            
            mcs.table_of_adsorbate_data[(mcs.number_of_adsorbates-1)*6+i][0]=0.0;
            
            
            
            // the process id
            
            mcs.table_of_adsorbate_data[(mcs.number_of_adsorbates-1)*6+i][9]=(double)((mcs.number_of_adsorbates-1)*6+i);
            
            
            
            mcs.sum_of_adsorption_rates=(NUMBER_OF_COLUMNS*NUMBER_OF_ROWS-mcs.number_of_adsorbates)*1.*mcs.deposition_rate;
            
            
            
            // write to the process counter for the adsorption process
            
            mcs.process_counter[7]+=1;
            
            mcs.rewrite_counter+=1;
            
            if(mcs.number_of_adsorbates==mcs.maximum_number_of_adsorbates){
                
                mcs.duration_of_growth=mcs.mc_time;
                mcs.growth_finished=true;
                
                
            }
            
        }
        
    }
    
}

double pick_random_diffusion_rate(struct MCS_STRUCT *ptr_to_mcs) {
    ptr_to_mcs = &mcs;
    
    // Prefetch the data we'll need to reduce cache misses
    int max_index = mcs.number_of_adsorbates_with_less_than_six_neigh*6;
    int current_index = max_index - mcs.diff_possibilities_evaluated_in_one_mc_step;
    int random_number = random_integer(current_index);
    
    // Single pointer swap instead of memory copies
    double *temp = mcs.table_of_adsorbate_data[random_number];
    double diffusion_direction = temp[7];
    double moving_adsorbate = temp[1];
    double diffusion_rate = temp[8];
    
    // Swap pointers
    mcs.table_of_adsorbate_data[random_number] = mcs.table_of_adsorbate_data[current_index];
    mcs.table_of_adsorbate_data[current_index] = temp;
    
    mcs.diff_possibilities_evaluated_in_one_mc_step++;
    mcs.diffusion_direction_selected = (int)diffusion_direction;
    mcs.id_of_moving_adsorbate = (int)moving_adsorbate;
    
    return diffusion_rate;
}


void  perform_diffusion(struct MCS_STRUCT *ptr_to_mcs_struct){
    
    ptr_to_mcs_struct=&mcs;
    
    int diffusion_direction=0,i_initial=0,j_initial=0,i_final=0,j_final=0;
    
    int i=0,ii=0;
    
    int number_of_atoms_in_vicinity=0;
    
    double temporary_sum_of_diff_rates=0.0;
    
    bool adsorbate_already_in_influenced_list=false;
    
    mcs.diff_possibilities_evaluated_in_one_mc_step=0;
    
    int line_number_in_table_of_adsorbate_data=0;
    // this is the line number where the selected processes is located.
    
    int list_of_line_numbers_for_one_adsorbate[7];
    
    
    for (i=1; i<=mcs.number_of_adsorbates_with_less_than_six_neigh*6; i++) {
        
        temporary_sum_of_diff_rates+=pick_random_diffusion_rate(&mcs);
        
        
        if (temporary_sum_of_diff_rates>mcs.random_number*mcs.sum_of_diffusion_rates) {
            
            
            // get the line number of the picked process
            
            line_number_in_table_of_adsorbate_data=mcs.number_of_adsorbates_with_less_than_six_neigh*6-(mcs.diff_possibilities_evaluated_in_one_mc_step-1);
            //mcs.diff_possibilities_evaluated_in_one_mc_step-1 is subtracted since the procedure pick_random_diffusion_rate increases this counter at the end. Take care for the -1
            
            
            //get the adsorbate id.
            
            mcs.id_of_moving_adsorbate=(int) (mcs.table_of_adsorbate_data[line_number_in_table_of_adsorbate_data][1]);
            
            // get the initial i and j coordinates
            
            i_initial=(int)( mcs.table_of_adsorbate_data[line_number_in_table_of_adsorbate_data][2]);
            
            j_initial=(int) (mcs.table_of_adsorbate_data[line_number_in_table_of_adsorbate_data][3]);
            
            // get the diffusion direction
            
            mcs.diffusion_direction_selected=(int)(  mcs.table_of_adsorbate_data[line_number_in_table_of_adsorbate_data][7] );
            
            
            
            
            break;
        }
        
        
    }
    
    
    
    
    
    diffusion_direction=mcs.diffusion_direction_selected;
    
    // write to the process counter
    
    mcs.process_counter[diffusion_direction]+=1;
    
    
    //  printf(" %d %d  %d \n",i_initial,j_initial,mcs.id_of_moving_adsorbate);
    
    
    
    
    if (diffusion_direction==1) {
        
        // diffusion direction 1
        //  i - >  i and j - > j+1
        
        i_final=i_periodic(i_initial,&mcs);
        j_final=j_periodic(j_initial+1,&mcs);
        
        
        
    }
    
    if (diffusion_direction==2) {
        
        // diffusion direction 2
        //  i - > i-1 and j - > j+1
        
        
        i_final=i_periodic(i_initial-1,&mcs);
        j_final=j_periodic(j_initial+1,&mcs);
        
        
        
        
    }
    
    if (diffusion_direction==3) {
        
        // diffusion direction 3
        //  i - > i-1 and j - > j
        
        
        i_final=i_periodic(i_initial-1,&mcs);
        j_final=j_periodic(j_initial,&mcs);
        
        
        
    }
    
    if (diffusion_direction==4) {
        
        // diffusion direction 4
        //  i - > i and j - > j-1
        
        
        i_final=i_periodic(i_initial,&mcs);
        j_final=j_periodic(j_initial-1,&mcs);
        
        
        
    }
    
    if (diffusion_direction==5) {
        
        // diffusion direction 5
        //  i - > i+1 and j - > j-1
        
        
        i_final=i_periodic(i_initial+1,&mcs);
        j_final=j_periodic(j_initial-1,&mcs);
        
        
        
        
        
    }
    
    if (diffusion_direction==6) {
        
        // diffusion direction 6
        //  i - > i+1 and j - > j
        
        
        i_final=i_periodic(i_initial+1,&mcs);
        j_final=j_periodic(j_initial,&mcs);
        
        
        
    }
    
    
    
    
    
    
    
    // check at first  the following fact:
    
    // Does the atom which is going to be  moved  have any neighbours in its initial  position ??
    
    // If this is case, write the adsorbate id of  this adsorbate to the table of adsorbates influenced by diffusion, increase the counter number of atoms in vicinity by 1 .
    
    
    // neighbour 1 first order white circle
    
    if(free_occupied_detector(i_periodic(i_initial,&mcs),j_periodic( j_initial+1,&mcs), &mcs)==1 ){
        
        // now me must look for the adsorbate id of the atom which sits at i=i_initial and j=j_initial+1
        
        number_of_atoms_in_vicinity+=1;
        
        
        
        for (i=1; i<=mcs.number_of_adsorbates; i++) {
            if (mcs.table_of_adsorbate_data[i][2]==i_initial && mcs.table_of_adsorbate_data[i][3]==j_initial+1    ) {
                mcs.id_of_touched_adsorbate=mcs.table_of_adsorbate_data[i][1];
            }
            
            
        }
        
        
        
        mcs.table_of_adsorbates_influenced_by_diffusion[number_of_atoms_in_vicinity][1]=mcs.id_of_touched_adsorbate;
        
        
        
    }
    
    // neighbour 2 first order  white circle
    
    if(free_occupied_detector(i_periodic(i_initial-1,&mcs),j_periodic( j_initial+1,&mcs), &mcs)==1  ){
        
        number_of_atoms_in_vicinity+=1;
        
        
        for (i=1; i<=mcs.number_of_adsorbates; i++) {
            if (mcs.table_of_adsorbate_data[i][2]==i_initial-1 && mcs.table_of_adsorbate_data[i][3]==j_initial+1    ) {
                mcs.id_of_touched_adsorbate=mcs.table_of_adsorbate_data[i][1];
            }
            
            
        }
        
        
        mcs.table_of_adsorbates_influenced_by_diffusion[number_of_atoms_in_vicinity][1]=mcs.id_of_touched_adsorbate;
        
        
    }
    
    
    // neighbour 3 first order  white circle
    
    if(free_occupied_detector(i_periodic(i_initial-1,&mcs),j_periodic( j_initial,&mcs), &mcs)==1 ){
        
        number_of_atoms_in_vicinity+=1;
        
        for (i=1; i<=mcs.number_of_adsorbates; i++) {
            if (mcs.table_of_adsorbate_data[i][2]==i_initial-1 && mcs.table_of_adsorbate_data[i][3]==j_initial    ) {
                mcs.id_of_touched_adsorbate=mcs.table_of_adsorbate_data[i][1];
            }
            
            
        }
        
        
        
        
        mcs.table_of_adsorbates_influenced_by_diffusion[number_of_atoms_in_vicinity][1]=mcs.id_of_touched_adsorbate;
        
        
    }
    
    
    // neighbour 4 first order  white circle
    
    if(free_occupied_detector(i_periodic( i_initial,&mcs),j_periodic( j_initial-1,&mcs), &mcs)==1 ){
        
        number_of_atoms_in_vicinity+=1;
        
        
        for (i=1; i<=mcs.number_of_adsorbates; i++) {
            if (mcs.table_of_adsorbate_data[i][2]==i_initial && mcs.table_of_adsorbate_data[i][3]==j_initial-1    ) {
                mcs.id_of_touched_adsorbate=mcs.table_of_adsorbate_data[i][1];
            }
            
            
        }
        
        
        mcs.table_of_adsorbates_influenced_by_diffusion[number_of_atoms_in_vicinity][1]=mcs.id_of_touched_adsorbate;
        
        
    }
    
    
    // neighbour 5 first order   white circle
    
    if(free_occupied_detector(i_periodic( i_initial+1,&mcs),j_periodic( j_initial-1,&mcs), &mcs)==1 ){
        
        number_of_atoms_in_vicinity+=1;
        
        for (i=1; i<=mcs.number_of_adsorbates; i++) {
            if (mcs.table_of_adsorbate_data[i][2]==i_initial+1 && mcs.table_of_adsorbate_data[i][3]==j_initial-1    ) {
                mcs.id_of_touched_adsorbate=mcs.table_of_adsorbate_data[i][1];
            }
            
            
        }
        
        
        
        mcs.table_of_adsorbates_influenced_by_diffusion[number_of_atoms_in_vicinity][1]=mcs.id_of_touched_adsorbate;
        
        
        
    }
    
    
    // neighbour 6 first order  white circle
    
    if(free_occupied_detector(i_periodic( i_initial+1,&mcs),j_periodic( j_initial,&mcs), &mcs)==1  ){
        
        number_of_atoms_in_vicinity+=1;
        
        for (i=1; i<=mcs.number_of_adsorbates; i++) {
            if (mcs.table_of_adsorbate_data[i][2]==i_initial+1 && mcs.table_of_adsorbate_data[i][3]==j_initial   ) {
                mcs.id_of_touched_adsorbate=mcs.table_of_adsorbate_data[i][1];
            }
            
            
        }
        
        
        
        mcs.table_of_adsorbates_influenced_by_diffusion[number_of_atoms_in_vicinity][1]=mcs.id_of_touched_adsorbate;
        
        
    }
    
    
    
    
    // neighbour 1 second  order    // green circle
    
    if(free_occupied_detector(i_periodic( i_initial-1,&mcs),j_periodic( j_initial+2,&mcs), &mcs)==1){
        
        number_of_atoms_in_vicinity+=1;
        
        for (i=1; i<=mcs.number_of_adsorbates; i++) {
            if (mcs.table_of_adsorbate_data[i][2]==i_initial-1 && mcs.table_of_adsorbate_data[i][3]==j_initial+2    ) {
                mcs.id_of_touched_adsorbate=mcs.table_of_adsorbate_data[i][1];
            }
            
            
        }
        
        
        mcs.table_of_adsorbates_influenced_by_diffusion[number_of_atoms_in_vicinity][1]=mcs.id_of_touched_adsorbate;
        
        
    }
    
    
    
    // neighbour 2 second  order    // green circle
    
    if(free_occupied_detector(i_periodic( i_initial-2,&mcs),j_periodic( j_initial+1,&mcs), &mcs)==1){
        
        number_of_atoms_in_vicinity+=1;
        
        for (i=1; i<=mcs.number_of_adsorbates; i++) {
            if (mcs.table_of_adsorbate_data[i][2]==i_initial-2 && mcs.table_of_adsorbate_data[i][3]==j_initial+1    ) {
                mcs.id_of_touched_adsorbate=mcs.table_of_adsorbate_data[i][1];
            }
            
            
        }
        
        mcs.table_of_adsorbates_influenced_by_diffusion[number_of_atoms_in_vicinity][1]=mcs.id_of_touched_adsorbate;
        
        
    }
    
    
    
    // neighbour 3 second  order      // green circle
    
    if(free_occupied_detector(i_periodic( i_initial-1,&mcs),j_periodic( j_initial-1,&mcs), &mcs)==1){
        
        number_of_atoms_in_vicinity+=1;
        
        for (i=1; i<=mcs.number_of_adsorbates; i++) {
            if (mcs.table_of_adsorbate_data[i][2]==i_initial-1 && mcs.table_of_adsorbate_data[i][3]==j_initial-1    ) {
                mcs.id_of_touched_adsorbate=mcs.table_of_adsorbate_data[i][1];
            }
            
            
        }
        
        
        mcs.table_of_adsorbates_influenced_by_diffusion[number_of_atoms_in_vicinity][1]=mcs.id_of_touched_adsorbate;
        
        
    }
    
    
    // neighbour 4 second  order      // green circle
    
    if(free_occupied_detector(i_periodic( i_initial+1,&mcs),j_periodic( j_initial-2,&mcs), &mcs)==1){
        
        number_of_atoms_in_vicinity+=1;
        
        for (i=1; i<=mcs.number_of_adsorbates; i++) {
            if (mcs.table_of_adsorbate_data[i][2]==i_initial+1 && mcs.table_of_adsorbate_data[i][3]==j_initial-2    ) {
                mcs.id_of_touched_adsorbate=mcs.table_of_adsorbate_data[i][1];
            }
            
            
        }
        
        mcs.table_of_adsorbates_influenced_by_diffusion[number_of_atoms_in_vicinity][1]=mcs.id_of_touched_adsorbate;
        
        
    }
    
    // neighbour 5 second  order     // green circle
    
    if(free_occupied_detector(i_periodic( i_initial+2,&mcs),j_periodic( j_initial-1,&mcs), &mcs)==1){
        
        number_of_atoms_in_vicinity+=1;
        
        for (i=1; i<=mcs.number_of_adsorbates; i++) {
            if (mcs.table_of_adsorbate_data[i][2]==i_initial+2 && mcs.table_of_adsorbate_data[i][3]==j_initial-1    ) {
                mcs.id_of_touched_adsorbate=mcs.table_of_adsorbate_data[i][1];
            }
            
            
        }
        
        
        mcs.table_of_adsorbates_influenced_by_diffusion[number_of_atoms_in_vicinity][1]=mcs.id_of_touched_adsorbate;
        
        
        
    }
    
    // neighbour 6 second  order // green circle
    
    if(free_occupied_detector(i_periodic( i_initial+1,&mcs),j_periodic( j_initial+1,&mcs), &mcs)==1){
        
        number_of_atoms_in_vicinity+=1;
        
        for (i=1; i<=mcs.number_of_adsorbates; i++) {
            if (mcs.table_of_adsorbate_data[i][2]==i_initial+1 && mcs.table_of_adsorbate_data[i][3]==j_initial+1    ) {
                mcs.id_of_touched_adsorbate=mcs.table_of_adsorbate_data[i][1];
            }
            
            
        }
        
        
        mcs.table_of_adsorbates_influenced_by_diffusion[number_of_atoms_in_vicinity][1]=mcs.id_of_touched_adsorbate;
        
        
    }
    
    
    
    
    // neighbour 1 third  order  // blue circle
    
    if(free_occupied_detector(i_periodic( i_initial,&mcs),j_periodic( j_initial+2,&mcs), &mcs)==1){
        
        number_of_atoms_in_vicinity+=1;
        
        for (i=1; i<=mcs.number_of_adsorbates; i++) {
            if (mcs.table_of_adsorbate_data[i][2]==i_initial && mcs.table_of_adsorbate_data[i][3]==j_initial+2 ) {
                mcs.id_of_touched_adsorbate=mcs.table_of_adsorbate_data[i][1];
            }
            
            
        }
        
        mcs.table_of_adsorbates_influenced_by_diffusion[number_of_atoms_in_vicinity][1]=mcs.id_of_touched_adsorbate;
        
        
    }
    
    
    
    // neighbour 2 third  order  // blue circle
    
    if(free_occupied_detector(i_periodic( i_initial-2,&mcs),j_periodic( j_initial+2,&mcs), &mcs)==1){
        
        number_of_atoms_in_vicinity+=1;
        
        for (i=1; i<=mcs.number_of_adsorbates; i++) {
            if (mcs.table_of_adsorbate_data[i][2]==i_initial-2 && mcs.table_of_adsorbate_data[i][3]==j_initial+2    ) {
                mcs.id_of_touched_adsorbate=mcs.table_of_adsorbate_data[i][1];
            }
            
            
        }
        
        mcs.table_of_adsorbates_influenced_by_diffusion[number_of_atoms_in_vicinity][1]=mcs.id_of_touched_adsorbate;
        
        
    }
    
    // neighbour 3 third  order  // blue circle
    
    if(free_occupied_detector(i_periodic( i_initial-2,&mcs),j_periodic( j_initial,&mcs), &mcs)==1){
        
        number_of_atoms_in_vicinity+=1;
        
        for (i=1; i<=mcs.number_of_adsorbates; i++) {
            if (mcs.table_of_adsorbate_data[i][2]==i_initial-2 && mcs.table_of_adsorbate_data[i][3]==j_initial   ) {
                mcs.id_of_touched_adsorbate=mcs.table_of_adsorbate_data[i][1];
            }
            
            
        }
        
        
        mcs.table_of_adsorbates_influenced_by_diffusion[number_of_atoms_in_vicinity][1]=mcs.id_of_touched_adsorbate;
        
    }
    
    
    // neighbour 4 third  order  // blue circle
    
    if(free_occupied_detector(i_periodic( i_initial,&mcs),j_periodic( j_initial-2,&mcs), &mcs)==1){
        
        number_of_atoms_in_vicinity+=1;
        
        for (i=1; i<=mcs.number_of_adsorbates; i++) {
            if (mcs.table_of_adsorbate_data[i][2]==i_initial && mcs.table_of_adsorbate_data[i][3]==j_initial-2    ) {
                mcs.id_of_touched_adsorbate=mcs.table_of_adsorbate_data[i][1];
            }
            
            
        }
        
        mcs.table_of_adsorbates_influenced_by_diffusion[number_of_atoms_in_vicinity][1]=mcs.id_of_touched_adsorbate;
        
        
    }
    
    
    // neighbour 5 third  order  // blue circle
    
    if(free_occupied_detector(i_periodic( i_initial+2,&mcs),j_periodic( j_initial-2,&mcs), &mcs)==1){
        
        number_of_atoms_in_vicinity+=1;
        
        for (i=1; i<=mcs.number_of_adsorbates; i++) {
            if (mcs.table_of_adsorbate_data[i][2]==i_initial+2 && mcs.table_of_adsorbate_data[i][3]==j_initial-2    ) {
                mcs.id_of_touched_adsorbate=mcs.table_of_adsorbate_data[i][1];
            }
            
            
        }
        
        
        mcs.table_of_adsorbates_influenced_by_diffusion[number_of_atoms_in_vicinity][1]=mcs.id_of_touched_adsorbate;
        
        
    }
    
    
    // neighbour 6 third  order  // blue circle
    
    if(free_occupied_detector(i_periodic( i_initial+2,&mcs),j_periodic( j_initial,&mcs), &mcs)==1){
        
        number_of_atoms_in_vicinity+=1;
        
        for (i=1; i<=mcs.number_of_adsorbates; i++) {
            if (mcs.table_of_adsorbate_data[i][2]==i_initial+2 && mcs.table_of_adsorbate_data[i][3]==j_initial  ) {
                mcs.id_of_touched_adsorbate=mcs.table_of_adsorbate_data[i][1];
            }
            
            
        }
        
        
        mcs.table_of_adsorbates_influenced_by_diffusion[number_of_atoms_in_vicinity][1]=mcs.id_of_touched_adsorbate;
        
        
        
        
    }
    
    
    
    
    // now do the same for the final position
    
    
    // in this case it would be possible that an adsorbate which is marked to be rewritten by a neighbour change of the initial position is remarked again by a neighbour change in the final position
    
    // Thus the following line is added to the code
    
    //for (i=1; i<=number_of_atoms_in_vicinity && adsorbate_already_in_influenced_list==false; i++){
    //if (mcs.table_of_adsorbates_influenced_by_diffusion[i][3]==mcs.id_of_touched_adsorbate) {
    //adsorbate_already_in_influenced_list=true;}
    //}
    
    // if (adsorbate_already_in_influenced_list==false) {
    
    
    
    
    // neighbour 1 first order
    
    if(free_occupied_detector(i_periodic(i_final,&mcs),j_periodic(j_final+1,&mcs), &mcs)==1){
        
        adsorbate_already_in_influenced_list=false;
        
        for (i=1; i<=mcs.number_of_adsorbates; i++) {
            if (mcs.table_of_adsorbate_data[i][2]==i_final && mcs.table_of_adsorbate_data[i][3]==j_final+1    ) {
                mcs.id_of_touched_adsorbate=mcs.table_of_adsorbate_data[i][1];
            }
            
        }
        
        
        
        for (i=1; i<=number_of_atoms_in_vicinity && adsorbate_already_in_influenced_list==false; i++){
            if (mcs.table_of_adsorbates_influenced_by_diffusion[i][1]==mcs.id_of_touched_adsorbate) {
                adsorbate_already_in_influenced_list=true;}
        }
        
        if (adsorbate_already_in_influenced_list==false) {
            
            
            number_of_atoms_in_vicinity+=1;
            mcs.table_of_adsorbates_influenced_by_diffusion[number_of_atoms_in_vicinity][1]=mcs.id_of_touched_adsorbate;
            
            
            
        }
        
        
    }
    
    // neighbour 2 first order
    
    if(free_occupied_detector(i_periodic(i_final-1,&mcs),j_periodic(j_final+1,&mcs), &mcs)==1){
        
        adsorbate_already_in_influenced_list=false;
        
        for (i=1; i<=number_of_atoms_in_vicinity && adsorbate_already_in_influenced_list==false; i++) {
            if (mcs.table_of_adsorbate_data[i][2]==i_final-1 && mcs.table_of_adsorbate_data[i][3]==j_final+1    ) {
                mcs.id_of_touched_adsorbate=mcs.table_of_adsorbate_data[i][1];
            }
            
        }
        
        for (i=1; i<=number_of_atoms_in_vicinity && adsorbate_already_in_influenced_list==false; i++){
            if (mcs.table_of_adsorbates_influenced_by_diffusion[i][1]==mcs.id_of_touched_adsorbate) {
                adsorbate_already_in_influenced_list=true;}
        }
        
        
        
        if (adsorbate_already_in_influenced_list==false) {
            
            number_of_atoms_in_vicinity+=1;
            mcs.table_of_adsorbates_influenced_by_diffusion[number_of_atoms_in_vicinity][1]=mcs.id_of_touched_adsorbate;
            
        }
        
    }
    
    // neighbour 3 first order
    
    if(free_occupied_detector(i_periodic(i_final-1,&mcs),j_periodic(j_final,&mcs), &mcs)==1){
        
        adsorbate_already_in_influenced_list=false;
        
        for (i=1; i<=mcs.number_of_adsorbates; i++) {
            if (mcs.table_of_adsorbate_data[i][2]==i_final-1 && mcs.table_of_adsorbate_data[i][3]==j_final    ) {
                mcs.id_of_touched_adsorbate=mcs.table_of_adsorbate_data[i][1];
            }
            
        }
        
        
        
        for (i=1; i<=number_of_atoms_in_vicinity && adsorbate_already_in_influenced_list==false; i++){
            if (mcs.table_of_adsorbates_influenced_by_diffusion[i][1]==mcs.id_of_touched_adsorbate) {
                adsorbate_already_in_influenced_list=true;}
        }
        
        if (adsorbate_already_in_influenced_list==false) {
            
            
            number_of_atoms_in_vicinity+=1;
            mcs.table_of_adsorbates_influenced_by_diffusion[number_of_atoms_in_vicinity][1]=mcs.id_of_touched_adsorbate;
            
            
            
        }
        
        
    }
    
    
    // neighbour 4 first order
    
    if(free_occupied_detector(i_periodic(i_final,&mcs),j_periodic(j_final-1,&mcs), &mcs)==1){
        
        adsorbate_already_in_influenced_list=false;
        
        for (i=1; i<=mcs.number_of_adsorbates; i++) {
            if (mcs.table_of_adsorbate_data[i][2]==i_final && mcs.table_of_adsorbate_data[i][3]==j_final-1    ) {
                mcs.id_of_touched_adsorbate=mcs.table_of_adsorbate_data[i][1];
            }
            
        }
        
        
        
        for (i=1; i<=number_of_atoms_in_vicinity && adsorbate_already_in_influenced_list==false; i++){
            if (mcs.table_of_adsorbates_influenced_by_diffusion[i][1]==mcs.id_of_touched_adsorbate) {
                adsorbate_already_in_influenced_list=true;}
        }
        
        if (adsorbate_already_in_influenced_list==false) {
            
            
            number_of_atoms_in_vicinity+=1;
            mcs.table_of_adsorbates_influenced_by_diffusion[number_of_atoms_in_vicinity][1]=mcs.id_of_touched_adsorbate;
            
            
            
        }
        
        
        
        
        
    }
    
    
    // neighbour 5 first order
    
    if(free_occupied_detector(i_periodic(i_final+1,&mcs),j_periodic(j_final-1,&mcs), &mcs)==1){
        
        
        adsorbate_already_in_influenced_list=false;
        
        for (i=1; i<=mcs.number_of_adsorbates; i++) {
            if (mcs.table_of_adsorbate_data[i][2]==i_final+1 && mcs.table_of_adsorbate_data[i][3]==j_final-1    ) {
                mcs.id_of_touched_adsorbate=mcs.table_of_adsorbate_data[i][1];
            }
            
        }
        
        
        
        for (i=1; i<=number_of_atoms_in_vicinity && adsorbate_already_in_influenced_list==false; i++){
            if (mcs.table_of_adsorbates_influenced_by_diffusion[i][1]==mcs.id_of_touched_adsorbate) {
                adsorbate_already_in_influenced_list=true;}
        }
        
        if (adsorbate_already_in_influenced_list==false) {
            
            
            number_of_atoms_in_vicinity+=1;
            mcs.table_of_adsorbates_influenced_by_diffusion[number_of_atoms_in_vicinity][1]=mcs.id_of_touched_adsorbate;
            
            
            
        }
        
        
        
        
        
    }
    
    
    // neighbour 6 first order
    
    if(free_occupied_detector(i_periodic(i_final+1,&mcs),j_periodic(j_final,&mcs), &mcs)==1){
        
        adsorbate_already_in_influenced_list=false;
        
        for (i=1; i<=mcs.number_of_adsorbates; i++) {
            if (mcs.table_of_adsorbate_data[i][2]==i_final+1 && mcs.table_of_adsorbate_data[i][3]==j_final    ) {
                mcs.id_of_touched_adsorbate=mcs.table_of_adsorbate_data[i][1];
            }
            
        }
        
        
        
        for (i=1; i<=number_of_atoms_in_vicinity && adsorbate_already_in_influenced_list==false; i++){
            if (mcs.table_of_adsorbates_influenced_by_diffusion[i][1]==mcs.id_of_touched_adsorbate) {
                adsorbate_already_in_influenced_list=true;}
        }
        
        if (adsorbate_already_in_influenced_list==false) {
            
            
            number_of_atoms_in_vicinity+=1;
            mcs.table_of_adsorbates_influenced_by_diffusion[number_of_atoms_in_vicinity][1]=mcs.id_of_touched_adsorbate;
            
            
            
        }
        
        
    }
    // neighbour 1 second  order    // green circle
    
    if(free_occupied_detector(i_periodic(i_final-1,&mcs),j_periodic(j_final+2,&mcs), &mcs)==1){
        
        adsorbate_already_in_influenced_list=false;
        
        for (i=1; i<=mcs.number_of_adsorbates; i++) {
            if (mcs.table_of_adsorbate_data[i][2]==i_final-1 && mcs.table_of_adsorbate_data[i][3]==j_final+2    ) {
                mcs.id_of_touched_adsorbate=mcs.table_of_adsorbate_data[i][1];
            }
            
        }
        
        
        
        for (i=1; i<=number_of_atoms_in_vicinity && adsorbate_already_in_influenced_list==false; i++){
            if (mcs.table_of_adsorbates_influenced_by_diffusion[i][1]==mcs.id_of_touched_adsorbate) {
                adsorbate_already_in_influenced_list=true;}
        }
        
        if (adsorbate_already_in_influenced_list==false) {
            
            
            number_of_atoms_in_vicinity+=1;
            mcs.table_of_adsorbates_influenced_by_diffusion[number_of_atoms_in_vicinity][1]=mcs.id_of_touched_adsorbate;
            
            
            
        }
        
        
    }
    
    
    // neighbour 2 second  order
    
    if(free_occupied_detector(i_periodic(i_final-2,&mcs),j_periodic(j_final+1,&mcs), &mcs)==1){
        
        adsorbate_already_in_influenced_list=false;
        
        for (i=1; i<=mcs.number_of_adsorbates; i++) {
            if (mcs.table_of_adsorbate_data[i][2]==i_final-2 && mcs.table_of_adsorbate_data[i][3]==j_final+1    ) {
                mcs.id_of_touched_adsorbate=mcs.table_of_adsorbate_data[i][1];
            }
            
        }
        
        
        
        for (i=1; i<=number_of_atoms_in_vicinity && adsorbate_already_in_influenced_list==false; i++){
            if (mcs.table_of_adsorbates_influenced_by_diffusion[i][1]==mcs.id_of_touched_adsorbate) {
                adsorbate_already_in_influenced_list=true;}
        }
        
        if (adsorbate_already_in_influenced_list==false) {
            
            
            number_of_atoms_in_vicinity+=1;
            mcs.table_of_adsorbates_influenced_by_diffusion[number_of_atoms_in_vicinity][1]=mcs.id_of_touched_adsorbate;
            
            
            
        }
        
    }
    
    
    // neighbour 3 second  order
    
    if(free_occupied_detector(i_periodic(i_final-1,&mcs),j_periodic(j_final+1,&mcs), &mcs)==1){
        
        adsorbate_already_in_influenced_list=false;
        
        for (i=1; i<=mcs.number_of_adsorbates; i++) {
            if (mcs.table_of_adsorbate_data[i][2]==i_final-1 && mcs.table_of_adsorbate_data[i][3]==j_final+1    ) {
                mcs.id_of_touched_adsorbate=mcs.table_of_adsorbate_data[i][1];
            }
            
        }
        
        
        
        for (i=1; i<=number_of_atoms_in_vicinity && adsorbate_already_in_influenced_list==false; i++){
            if (mcs.table_of_adsorbates_influenced_by_diffusion[i][1]==mcs.id_of_touched_adsorbate) {
                adsorbate_already_in_influenced_list=true;}
        }
        
        if (adsorbate_already_in_influenced_list==false) {
            
            
            number_of_atoms_in_vicinity+=1;
            mcs.table_of_adsorbates_influenced_by_diffusion[number_of_atoms_in_vicinity][1]=mcs.id_of_touched_adsorbate;
            
            
            
        }
        
        
        
        
    }
    
    // neighbour 4 second  order
    
    if(free_occupied_detector(i_periodic(i_final+1,&mcs),j_periodic(j_final-2,&mcs), &mcs)==1){
        
        adsorbate_already_in_influenced_list=false;
        
        for (i=1; i<=mcs.number_of_adsorbates; i++) {
            if (mcs.table_of_adsorbate_data[i][2]==i_final+1 && mcs.table_of_adsorbate_data[i][3]==j_final-2    ) {
                mcs.id_of_touched_adsorbate=mcs.table_of_adsorbate_data[i][1];
            }
            
        }
        
        
        
        for (i=1; i<=number_of_atoms_in_vicinity && adsorbate_already_in_influenced_list==false; i++){
            if (mcs.table_of_adsorbates_influenced_by_diffusion[i][1]==mcs.id_of_touched_adsorbate) {
                adsorbate_already_in_influenced_list=true;}
        }
        
        if (adsorbate_already_in_influenced_list==false) {
            
            
            number_of_atoms_in_vicinity+=1;
            mcs.table_of_adsorbates_influenced_by_diffusion[number_of_atoms_in_vicinity][1]=mcs.id_of_touched_adsorbate;
            
            
            
        }
        
        
    }
    
    // neighbour 5 second  order
    
    if(free_occupied_detector(i_periodic(i_final+2,&mcs),j_periodic(j_final-1,&mcs), &mcs)==1){
        
        adsorbate_already_in_influenced_list=false;
        
        for (i=1; i<=mcs.number_of_adsorbates; i++) {
            if (mcs.table_of_adsorbate_data[i][2]==i_final+2 && mcs.table_of_adsorbate_data[i][3]==j_final-1    ) {
                mcs.id_of_touched_adsorbate=mcs.table_of_adsorbate_data[i][1];
            }
            
        }
        
        
        
        for (i=1; i<=number_of_atoms_in_vicinity && adsorbate_already_in_influenced_list==false; i++){
            if (mcs.table_of_adsorbates_influenced_by_diffusion[i][1]==mcs.id_of_touched_adsorbate) {
                adsorbate_already_in_influenced_list=true;}
        }
        
        if (adsorbate_already_in_influenced_list==false) {
            
            
            number_of_atoms_in_vicinity+=1;
            mcs.table_of_adsorbates_influenced_by_diffusion[number_of_atoms_in_vicinity][1]=mcs.id_of_touched_adsorbate;
            
            
            
        }
        
        
    }
    
    // neighbour 6 second  order
    
    if(free_occupied_detector(i_periodic(i_final+1,&mcs),j_periodic(j_final+1,&mcs), &mcs)==1){
        
        adsorbate_already_in_influenced_list=false;
        
        for (i=1; i<=mcs.number_of_adsorbates; i++) {
            if (mcs.table_of_adsorbate_data[i][2]==i_final+1 && mcs.table_of_adsorbate_data[i][3]==j_final+1    ) {
                mcs.id_of_touched_adsorbate=mcs.table_of_adsorbate_data[i][1];
            }
            
        }
        
        
        
        for (i=1; i<=number_of_atoms_in_vicinity && adsorbate_already_in_influenced_list==false; i++){
            if (mcs.table_of_adsorbates_influenced_by_diffusion[i][1]==mcs.id_of_touched_adsorbate) {
                adsorbate_already_in_influenced_list=true;}
        }
        
        if (adsorbate_already_in_influenced_list==false) {
            
            
            number_of_atoms_in_vicinity+=1;
            mcs.table_of_adsorbates_influenced_by_diffusion[number_of_atoms_in_vicinity][1]=mcs.id_of_touched_adsorbate;
            
            
            
        }
        
        
    }
    
    
    // neighbour 1 third  order  // blue circle
    
    if(free_occupied_detector(i_periodic(i_final,&mcs),j_periodic(j_final+2,&mcs), &mcs)==1){
        
        adsorbate_already_in_influenced_list=false;
        
        for (i=1; i<=mcs.number_of_adsorbates; i++) {
            if (mcs.table_of_adsorbate_data[i][2]==i_final && mcs.table_of_adsorbate_data[i][3]==j_final+2    ) {
                mcs.id_of_touched_adsorbate=mcs.table_of_adsorbate_data[i][1];
            }
            
        }
        
        
        
        for (i=1; i<=number_of_atoms_in_vicinity && adsorbate_already_in_influenced_list==false; i++){
            if (mcs.table_of_adsorbates_influenced_by_diffusion[i][1]==mcs.id_of_touched_adsorbate) {
                adsorbate_already_in_influenced_list=true;}
        }
        
        if (adsorbate_already_in_influenced_list==false) {
            
            
            number_of_atoms_in_vicinity+=1;
            mcs.table_of_adsorbates_influenced_by_diffusion[number_of_atoms_in_vicinity][1]=mcs.id_of_touched_adsorbate;
            
            
            
        }
        
        
    }
    
    
    // neighbour 2 third  order  // blue circle
    
    if(free_occupied_detector(i_periodic(i_final-2,&mcs),j_periodic(j_final+2,&mcs), &mcs)==1){
        
        adsorbate_already_in_influenced_list=false;
        
        for (i=1; i<=mcs.number_of_adsorbates; i++) {
            if (mcs.table_of_adsorbate_data[i][2]==i_final-2 && mcs.table_of_adsorbate_data[i][3]==j_final+2    ) {
                mcs.id_of_touched_adsorbate=mcs.table_of_adsorbate_data[i][1];
            }
            
        }
        
        
        
        for (i=1; i<=number_of_atoms_in_vicinity && adsorbate_already_in_influenced_list==false; i++){
            if (mcs.table_of_adsorbates_influenced_by_diffusion[i][1]==mcs.id_of_touched_adsorbate) {
                adsorbate_already_in_influenced_list=true;}
        }
        
        if (adsorbate_already_in_influenced_list==false) {
            
            
            number_of_atoms_in_vicinity+=1;
            mcs.table_of_adsorbates_influenced_by_diffusion[number_of_atoms_in_vicinity][1]=mcs.id_of_touched_adsorbate;
            
            
            
        }
        
        
        
        
        
        
        
    }
    
    // neighbour 3 third  order  // blue circle
    
    if(free_occupied_detector(i_periodic(i_final-2,&mcs),j_periodic(j_final,&mcs), &mcs)==1){
        
        adsorbate_already_in_influenced_list=false;
        
        for (i=1; i<=mcs.number_of_adsorbates; i++) {
            if (mcs.table_of_adsorbate_data[i][2]==i_final-2 && mcs.table_of_adsorbate_data[i][3]==j_final   ) {
                mcs.id_of_touched_adsorbate=mcs.table_of_adsorbate_data[i][1];
            }
            
        }
        
        
        
        for (i=1; i<=number_of_atoms_in_vicinity && adsorbate_already_in_influenced_list==false; i++){
            if (mcs.table_of_adsorbates_influenced_by_diffusion[i][1]==mcs.id_of_touched_adsorbate) {
                adsorbate_already_in_influenced_list=true;}
        }
        
        if (adsorbate_already_in_influenced_list==false) {
            
            
            number_of_atoms_in_vicinity+=1;
            mcs.table_of_adsorbates_influenced_by_diffusion[number_of_atoms_in_vicinity][1]=mcs.id_of_touched_adsorbate;
            
            
            
        }
        
        
        
        
        
        
    }
    
    
    // neighbour 4 third  order  // blue circle
    
    if(free_occupied_detector(i_periodic(i_final,&mcs),j_periodic(j_final-2,&mcs), &mcs)==1){
        
        adsorbate_already_in_influenced_list=false;
        
        for (i=1; i<=mcs.number_of_adsorbates; i++) {
            if (mcs.table_of_adsorbate_data[i][2]==i_final && mcs.table_of_adsorbate_data[i][3]==j_final-2    ) {
                mcs.id_of_touched_adsorbate=mcs.table_of_adsorbate_data[i][1];
            }
            
        }
        
        
        
        for (i=1; i<=number_of_atoms_in_vicinity && adsorbate_already_in_influenced_list==false; i++){
            if (mcs.table_of_adsorbates_influenced_by_diffusion[i][1]==mcs.id_of_touched_adsorbate) {
                adsorbate_already_in_influenced_list=true;}
        }
        
        if (adsorbate_already_in_influenced_list==false) {
            
            
            number_of_atoms_in_vicinity+=1;
            mcs.table_of_adsorbates_influenced_by_diffusion[number_of_atoms_in_vicinity][1]=mcs.id_of_touched_adsorbate;
            
            
            
        }
        
        
        
        
        
    }
    
    
    // neighbour 5 third  order  // blue circle
    
    if(free_occupied_detector(i_periodic(i_final+2,&mcs),j_periodic(j_final-2,&mcs), &mcs)==1){
        
        adsorbate_already_in_influenced_list=false;
        
        for (i=1; i<=mcs.number_of_adsorbates; i++) {
            if (mcs.table_of_adsorbate_data[i][2]==i_final+2 && mcs.table_of_adsorbate_data[i][3]==j_final-2    ) {
                mcs.id_of_touched_adsorbate=mcs.table_of_adsorbate_data[i][1];
            }
            
        }
        
        
        
        for (i=1; i<=number_of_atoms_in_vicinity && adsorbate_already_in_influenced_list==false; i++){
            if (mcs.table_of_adsorbates_influenced_by_diffusion[i][1]==mcs.id_of_touched_adsorbate) {
                adsorbate_already_in_influenced_list=true;}
        }
        
        if (adsorbate_already_in_influenced_list==false) {
            
            
            number_of_atoms_in_vicinity+=1;
            mcs.table_of_adsorbates_influenced_by_diffusion[number_of_atoms_in_vicinity][1]=mcs.id_of_touched_adsorbate;
            
            
            
        }
        
        
        
    }
    
    // neighbour 6 third  order  // blue circle
    
    if(free_occupied_detector(i_periodic(i_final+2,&mcs),j_periodic(j_final,&mcs), &mcs)==1){
        
        adsorbate_already_in_influenced_list=false;
        
        for (i=1; i<=mcs.number_of_adsorbates; i++) {
            if (mcs.table_of_adsorbate_data[i][2]==i_final+2 && mcs.table_of_adsorbate_data[i][3]==j_final ) {
                mcs.id_of_touched_adsorbate=mcs.table_of_adsorbate_data[i][1];
            }
            
        }
        
        
        
        for (i=1; i<=number_of_atoms_in_vicinity && adsorbate_already_in_influenced_list==false; i++){
            if (mcs.table_of_adsorbates_influenced_by_diffusion[i][1]==mcs.id_of_touched_adsorbate) {
                adsorbate_already_in_influenced_list=true;}
        }
        
        if (adsorbate_already_in_influenced_list==false) {
            
            
            number_of_atoms_in_vicinity+=1;
            mcs.table_of_adsorbates_influenced_by_diffusion[number_of_atoms_in_vicinity][1]=mcs.id_of_touched_adsorbate;
            
            
            
        }
        
        
    }
    
    
    
    
    
    if(number_of_atoms_in_vicinity>1){
        
        mcs.rewrite_data_for_selected_atoms_necessary=true;
        
        mcs.rewrite_counter+=1;
        
        mcs.number_of_adsorbates_influenced_by_diffusion=number_of_atoms_in_vicinity;
        
        
        
        
        
    }
    
    
    else{
        
        
        mcs.rewrite_data_for_selected_atoms_necessary=false;
        
        
    }
    
    
    
    // After all these checks having been done the atom has to be moved really
    
    // This should be done at the very end of this procedure
    
    // now write the final coordinates to all the six entries of the table of adsorbate data
    
    // so get all the six line numbers
    
    ii=0;
    
    
    for (i=1; i<=mcs.number_of_adsorbates*6; i++) {
        
        if ((int) (mcs.table_of_adsorbate_data[i][1]) == mcs.id_of_moving_adsorbate ) {
            
            ii+=1;
            
            list_of_line_numbers_for_one_adsorbate[ii]=i;
            
            
        }
        
        
        
    }
    
    
    // check whether all six entries for the selected adsorbate have been found
    
    if (ii< 6) {
        
        
        
        
        printf("error in table of adsorbate data during diffusion processes. Not all six entries for the adsorbate with the adsorbate id %d  have been found. \n  ",mcs.id_of_moving_adsorbate);
        
    }
    
    // now write the new i and j coordinates to the lines of the table of adsorbate data
    // also this at the very end of the procedure
    
    
    for (ii=1; ii<=6; ii++) {
        
        mcs.table_of_adsorbate_data[list_of_line_numbers_for_one_adsorbate[ii]][2]=i_final;
        mcs.table_of_adsorbate_data[list_of_line_numbers_for_one_adsorbate[ii]][3]=j_final;
        
        
    }
    
    
    
    
    
    
    
}


void  rewrite_all_neighbours_and_rates_after_adsorption(struct MCS_STRUCT *ptr_to_mcs_struct){
    
    ptr_to_mcs_struct=&mcs;
    
    int neighbours_at_initial_position_first_order=0,neighbours_at_initial_position_second_order=0,neighbours_at_initial_position_third_order=0,
    neighbours_at_final_position_first_order=0,
    neighbours_at_final_position_second_order=0,
    neighbours_at_final_position_third_order=0;
    
    
    double diffusion_activation_energy_influenced_by_neighbours=0.0,energy_difference_intial_final_position=0.0,local_activation_energy=0.0,diffusion_rate=0.0;
    
    int diffusion_direction=0,i_initial=0,j_initial=0,adsorbate_index_selected=0,i_final_position=0,j_final_position=0,on_top_diffusion_i_coordinate=0,on_top_diffusion_j_coordinate=0;
    
    
    int i=0,ii=0;
    
    int adsorbate_id;
    
    
    mcs.sorting_and_counting_necessary=false;
    
    int list_of_line_numbers_for_one_adsorbate[7];
    
    // int line_number_in_table_of_adsorbate_data;
    
    int number_of_entries_for_one_adsorbate=0;
    
    int old_number_of_neighbours_first_order=0;
    
    // rewrite the neighbour list for all atoms
    
    
    for (adsorbate_id=1; adsorbate_id<=mcs.number_of_adsorbates; adsorbate_id++) {
        
        number_of_entries_for_one_adsorbate=0;
        
        for (i=1; i<=mcs.number_of_adsorbates*6 && number_of_entries_for_one_adsorbate<=6; i++) {
            
            
            if ( adsorbate_id == (int) (mcs.table_of_adsorbate_data[i][1] )   ) {
                
                number_of_entries_for_one_adsorbate+=1;
                
                list_of_line_numbers_for_one_adsorbate[number_of_entries_for_one_adsorbate]=i;
                
            }
            
        }
        
        
        //
        
        if (number_of_entries_for_one_adsorbate<6) {
            
            
            
            
            printf(" number of entries for one adsorbate is smaller than 6 \n Error in rewrite all neighbours and rates \n" );
            
        }
        
        
        
        
        
        
        // diagnosis think about renaming i_selected to i_initial and j in the same way.
        
        
        for (ii=1; ii<=6; ii++) {
            // ii is the line index in list of line numbers
            
            
            if(ii==1){
                
                // take the first line where the adsorbate with the chosen adsorbate id is and rewrite the neighbours
                
                adsorbate_index_selected=  (int) (mcs.table_of_adsorbate_data[list_of_line_numbers_for_one_adsorbate[ii]][1] );
                i_initial= (int) (mcs.table_of_adsorbate_data[list_of_line_numbers_for_one_adsorbate[ii]][2] );
                j_initial=  (int) (mcs.table_of_adsorbate_data[list_of_line_numbers_for_one_adsorbate[ii]][3] );
                
                
                
                
                
                
                old_number_of_neighbours_first_order= (int) (mcs.table_of_adsorbate_data[list_of_line_numbers_for_one_adsorbate[ii]][4]);
                
                
                neighbours_at_initial_position_first_order=
                free_occupied_detector(i_periodic(i_initial+1,&mcs), j_periodic(j_initial,&mcs),&mcs)+
                free_occupied_detector(i_periodic(i_initial+1,&mcs), j_periodic(j_initial-1,&mcs),&mcs)+
                free_occupied_detector(i_periodic(i_initial,&mcs), j_periodic(j_initial-1,&mcs),&mcs)+
                free_occupied_detector(i_periodic(i_initial-1,&mcs), j_periodic(j_initial,&mcs),&mcs)+
                free_occupied_detector(i_periodic(i_initial-1,&mcs), j_periodic(j_initial+1,&mcs),&mcs)+
                free_occupied_detector(i_periodic(i_initial,&mcs), j_periodic(j_initial+1,&mcs),&mcs);
                
                
                neighbours_at_initial_position_second_order=
                free_occupied_detector(i_periodic(i_initial+2,&mcs), j_periodic(j_initial-1,&mcs),&mcs)+
                free_occupied_detector(i_periodic(i_initial+1,&mcs), j_periodic(j_initial-2,&mcs),&mcs)+
                free_occupied_detector(i_periodic(i_initial-1,&mcs), j_periodic(j_initial-1,&mcs),&mcs)+
                free_occupied_detector(i_periodic(i_initial-2,&mcs), j_periodic(j_initial+1,&mcs),&mcs)+
                free_occupied_detector(i_periodic(i_initial-1,&mcs), j_periodic(j_initial+2,&mcs),&mcs)+
                free_occupied_detector(i_periodic(i_initial+1,&mcs), j_periodic(j_initial+1,&mcs),&mcs);
                
                
                neighbours_at_initial_position_third_order=
                free_occupied_detector(i_periodic(i_initial+2,&mcs), j_periodic(j_initial,&mcs),&mcs)+
                free_occupied_detector(i_periodic(i_initial+2,&mcs), j_periodic(j_initial-2,&mcs),&mcs)+
                free_occupied_detector(i_periodic(i_initial,&mcs), j_periodic(j_initial-2,&mcs),&mcs)+
                free_occupied_detector(i_periodic(i_initial-2,&mcs), j_periodic(j_initial,&mcs),&mcs)+
                free_occupied_detector(i_periodic(i_initial-2,&mcs), j_periodic(j_initial+2,&mcs),&mcs)+
                free_occupied_detector(i_periodic(i_initial,&mcs), j_periodic(j_initial+2,&mcs),&mcs);
                
                
                // check now whether whether the number of neighbours in first order has been lower than 6 and is now 6 or whether the number of neighbours was 6 and is now lower than 6. In this case the count and sort procecures should run.
                
                
                if (old_number_of_neighbours_first_order<6 &&  neighbours_at_initial_position_first_order==6  ) {
                    mcs.sorting_and_counting_necessary=true;
                }
                
                if (old_number_of_neighbours_first_order==6 &&  neighbours_at_initial_position_first_order<6  ) {
                    
                    
                    mcs.sorting_and_counting_necessary=true;
                }
                
                
                
                
                
                mcs.table_of_adsorbate_data[list_of_line_numbers_for_one_adsorbate[ii]][4]=(double) (neighbours_at_initial_position_first_order);
                mcs.table_of_adsorbate_data[list_of_line_numbers_for_one_adsorbate[ii]][5]=(double) (neighbours_at_initial_position_second_order);
                mcs.table_of_adsorbate_data[list_of_line_numbers_for_one_adsorbate[ii]][6]=(double) (neighbours_at_initial_position_third_order);
                
                
            }
            
            
            if(ii>1){
                
                // for ii between 2 and 6 fill the number of neighours  at the other 5 lines
                
                mcs.table_of_adsorbate_data[list_of_line_numbers_for_one_adsorbate[ii]][1]=(double) (adsorbate_id);
                mcs.table_of_adsorbate_data[list_of_line_numbers_for_one_adsorbate[ii]][2]=(double) (i_initial);
                mcs.table_of_adsorbate_data[list_of_line_numbers_for_one_adsorbate[ii]][3]=(double) (j_initial);
                mcs.table_of_adsorbate_data[list_of_line_numbers_for_one_adsorbate[ii]][4]=(double) (neighbours_at_initial_position_first_order);
                mcs.table_of_adsorbate_data[list_of_line_numbers_for_one_adsorbate[ii]][5]=(double) (neighbours_at_initial_position_second_order);
                mcs.table_of_adsorbate_data[list_of_line_numbers_for_one_adsorbate[ii]][6]=(double) (neighbours_at_initial_position_third_order);
                
                
                
                
                
                
            }
            
            
        }
        
        
        
        // now write the six diffusion rates for the adsorbate with the coordinates i_selected and j_selected
        
        // for this get the diffusion directions of the atom with the values i_selected, j_selected, adsorbate_index_selected
        
        
        
        // get the diffusion direction
        
        
        for (ii=1; ii<=6; ii++) {
            
            // ii is the line index in list of line numbers
            
            
            diffusion_direction=(int)(mcs.table_of_adsorbate_data[list_of_line_numbers_for_one_adsorbate[ii]][7]);
            
            
            //  determine the number of neighbouors at the final position for the diffusion directions 1 to 6
            
            
            if (diffusion_direction==1) {
                
                // diffusion direction 1
                //  i-> i and  j-> j+1
                
                
                i_final_position=i_initial;   j_final_position=j_periodic(j_initial+1,&mcs);
                
                if (free_occupied_detector(i_final_position, j_final_position, &mcs)==1) {
                    
                    diffusion_rate=0.0;
                    
                    
                }
                
                else{
                    
                    
                    
                    
                    neighbours_at_final_position_first_order=
                    free_occupied_detector(i_periodic(i_final_position,&mcs), j_periodic(j_final_position+1,&mcs),&mcs)+
                    free_occupied_detector(i_periodic(i_final_position-1,&mcs), j_periodic(j_final_position+1,&mcs),&mcs)+
                    free_occupied_detector(i_periodic(i_final_position-1,&mcs), j_periodic(j_final_position,&mcs),&mcs)+
                    free_occupied_detector(i_periodic(i_final_position,&mcs), j_periodic(j_final_position-1,&mcs),&mcs)+
                    free_occupied_detector(i_periodic(i_final_position+1,&mcs), j_periodic(j_final_position-1,&mcs),&mcs)+
                    free_occupied_detector(i_periodic(i_final_position+1,&mcs), j_periodic(j_final_position,&mcs),&mcs)
                    -1;
                    
                    // The numbers of neighbours at final position must be reduced by 1  for the energy calculation since the diffusing atom is its own neighbour at final position otherwise
                    
                    
                    
                    neighbours_at_final_position_second_order=
                    free_occupied_detector(i_periodic(i_final_position-1,&mcs), j_periodic(j_final_position+2,&mcs),&mcs)+
                    free_occupied_detector(i_periodic(i_final_position-2,&mcs), j_periodic(j_final_position+1,&mcs),&mcs)+
                    free_occupied_detector(i_periodic(i_final_position-1,&mcs), j_periodic(j_final_position-1,&mcs),&mcs)+
                    free_occupied_detector(i_periodic(i_final_position+1,&mcs), j_periodic(j_final_position-2,&mcs),&mcs)+
                    free_occupied_detector(i_periodic(i_final_position+2,&mcs), j_periodic(j_final_position-1,&mcs),&mcs)+
                    free_occupied_detector(i_periodic(i_final_position+1,&mcs), j_periodic(j_final_position+1,&mcs),&mcs);
                    
                    
                    
                    neighbours_at_final_position_third_order=
                    free_occupied_detector(i_periodic(i_final_position,&mcs), j_periodic(j_final_position+2,&mcs),&mcs)+
                    free_occupied_detector(i_periodic(i_final_position-2,&mcs), j_periodic(j_final_position+2,&mcs),&mcs)+
                    free_occupied_detector(i_periodic(i_final_position-2,&mcs), j_periodic(j_final_position,&mcs),&mcs)+
                    free_occupied_detector(i_periodic(i_final_position,&mcs), j_periodic(j_final_position-2,&mcs),&mcs)+
                    free_occupied_detector(i_periodic(i_final_position+2,&mcs), j_periodic(j_final_position-2,&mcs),&mcs)+
                    free_occupied_detector(i_periodic(i_final_position+2,&mcs), j_periodic(j_final_position,&mcs),&mcs);
                    
                    
                    diffusion_activation_energy_influenced_by_neighbours=(DIFFUSION_ACTIVATION_ENERGY+(neighbours_at_initial_position_first_order*1.*NN_ENERGY)+(neighbours_at_initial_position_second_order*1.*NNN_ENERGY)+(neighbours_at_initial_position_third_order*1.*NNNN_ENERGY));
                    
                    
                    energy_difference_intial_final_position=(neighbours_at_initial_position_first_order-neighbours_at_final_position_first_order)*1.*NN_ENERGY+(neighbours_at_initial_position_second_order-neighbours_at_final_position_second_order)*1.*NNN_ENERGY+(neighbours_at_initial_position_third_order-neighbours_at_final_position_third_order)*1.*NNNN_ENERGY;
                    
                    
                    
                    
                    local_activation_energy=diffusion_activation_energy_influenced_by_neighbours+energy_difference_intial_final_position/BARRIER_DEFORMATION_FACTOR;
                    
                    diffusion_rate=PREEXPFACTOR_DIFFUSION
                    *exp(-(local_activation_energy*ELECTRON_CHARGE)/(K_BOLTZMANN*mcs.temperature));
                    
                    
                    
                    //  Now comes the hindering of on top diffusion
                    
                    on_top_diffusion_i_coordinate=i_periodic(i_initial+1,&mcs);
                    on_top_diffusion_j_coordinate=j_periodic(j_initial,&mcs);
                    
                    
                    
                    if(free_occupied_detector(on_top_diffusion_i_coordinate, on_top_diffusion_j_coordinate,&mcs)==1){
                        
                        
                        diffusion_rate=PREEXPFACTOR_DIFFUSION
                        *exp(-(VIA_ON_TOP_DIFFUSION_FACTOR*local_activation_energy*ELECTRON_CHARGE)/(K_BOLTZMANN*mcs.temperature));
                        
                    }
                }
                
                // now write it to the table of adsorbate data
                
                mcs.table_of_adsorbate_data[list_of_line_numbers_for_one_adsorbate[ii]][8]=diffusion_rate;
                
            }
            
            
            
            
            if (diffusion_direction==2) {
                
                // diffusion direction 2
                //  i-> i-1 and j-> j+1
                
                
                i_final_position=i_periodic(i_initial-1,&mcs);   j_final_position=j_periodic(j_initial+1,&mcs);
                
                
                if (free_occupied_detector(i_final_position, j_final_position, &mcs)==1) {
                    
                    diffusion_rate=0.0;
                    
                    
                }
                
                else{
                    
                    
                    
                    
                    neighbours_at_final_position_first_order=
                    free_occupied_detector(i_periodic(i_final_position,&mcs), j_periodic(j_final_position+1,&mcs),&mcs)+
                    free_occupied_detector(i_periodic(i_final_position-1,&mcs), j_periodic(j_final_position+1,&mcs),&mcs)+
                    free_occupied_detector(i_periodic(i_final_position-1,&mcs), j_periodic(j_final_position,&mcs),&mcs)+
                    free_occupied_detector(i_periodic(i_final_position,&mcs), j_periodic(j_final_position-1,&mcs),&mcs)+
                    free_occupied_detector(i_periodic(i_final_position+1,&mcs), j_periodic(j_final_position-1,&mcs),&mcs)+
                    free_occupied_detector(i_periodic(i_final_position+1,&mcs), j_periodic(j_final_position,&mcs),&mcs)
                    -1;
                    
                    // The numbers of neighbours at final position must be reduced by 1  for the energy calculation since the diffusing atom is its own neighbour at final position otherwise
                    
                    
                    
                    neighbours_at_final_position_second_order=
                    free_occupied_detector(i_periodic(i_final_position-1,&mcs), j_periodic(j_final_position+2,&mcs),&mcs)+
                    free_occupied_detector(i_periodic(i_final_position-2,&mcs), j_periodic(j_final_position+1,&mcs),&mcs)+
                    free_occupied_detector(i_periodic(i_final_position-1,&mcs), j_periodic(j_final_position-1,&mcs),&mcs)+
                    free_occupied_detector(i_periodic(i_final_position+1,&mcs), j_periodic(j_final_position-2,&mcs),&mcs)+
                    free_occupied_detector(i_periodic(i_final_position+2,&mcs), j_periodic(j_final_position-1,&mcs),&mcs)+
                    free_occupied_detector(i_periodic(i_final_position+1,&mcs), j_periodic(j_final_position+1,&mcs),&mcs);
                    
                    
                    
                    neighbours_at_final_position_third_order=
                    free_occupied_detector(i_periodic(i_final_position,&mcs), j_periodic(j_final_position+2,&mcs),&mcs)+
                    free_occupied_detector(i_periodic(i_final_position-2,&mcs), j_periodic(j_final_position+2,&mcs),&mcs)+
                    free_occupied_detector(i_periodic(i_final_position-2,&mcs), j_periodic(j_final_position,&mcs),&mcs)+
                    free_occupied_detector(i_periodic(i_final_position,&mcs), j_periodic(j_final_position-2,&mcs),&mcs)+
                    free_occupied_detector(i_periodic(i_final_position+2,&mcs), j_periodic(j_final_position-2,&mcs),&mcs)+
                    free_occupied_detector(i_periodic(i_final_position+2,&mcs), j_periodic(j_final_position,&mcs),&mcs);
                    
                    
                    diffusion_activation_energy_influenced_by_neighbours=(DIFFUSION_ACTIVATION_ENERGY+(neighbours_at_initial_position_first_order*1.*NN_ENERGY)+(neighbours_at_initial_position_second_order*1.*NNN_ENERGY)+(neighbours_at_initial_position_third_order*1.*NNNN_ENERGY));
                    
                    
                    energy_difference_intial_final_position=(neighbours_at_initial_position_first_order-neighbours_at_final_position_first_order)*1.*NN_ENERGY+(neighbours_at_initial_position_second_order-neighbours_at_final_position_second_order)*1.*NNN_ENERGY+(neighbours_at_initial_position_third_order-neighbours_at_final_position_third_order)*1.*NNNN_ENERGY;
                    
                    
                    
                    
                    local_activation_energy=diffusion_activation_energy_influenced_by_neighbours+energy_difference_intial_final_position/BARRIER_DEFORMATION_FACTOR;
                    
                    diffusion_rate=PREEXPFACTOR_DIFFUSION
                    *exp(-(local_activation_energy*ELECTRON_CHARGE)/(K_BOLTZMANN*mcs.temperature));
                    
                    
                    
                    //  Now comes the hindering of on top diffusion
                    
                    on_top_diffusion_i_coordinate=i_periodic(i_initial-1,&mcs);
                    on_top_diffusion_j_coordinate=j_periodic(j_initial,&mcs);
                    
                    
                    
                    if(free_occupied_detector(on_top_diffusion_i_coordinate, on_top_diffusion_j_coordinate,&mcs)==1){
                        
                        
                        
                        
                        diffusion_rate=PREEXPFACTOR_DIFFUSION
                        *exp(-(VIA_ON_TOP_DIFFUSION_FACTOR*local_activation_energy*ELECTRON_CHARGE)/(K_BOLTZMANN*mcs.temperature));
                        
                    }
                    
                }
                
                // now write it to the table of adsorbate data
                
                mcs.table_of_adsorbate_data[list_of_line_numbers_for_one_adsorbate[ii]][8]=diffusion_rate;
                
            }
            
            
            
            
            if (diffusion_direction==3) {
                
                // diffusion direction 3
                //  i-> i-1 and j-> j
                
                
                i_final_position=i_periodic(i_initial-1,&mcs);   j_final_position=j_initial;
                
                if (free_occupied_detector(i_final_position, j_final_position, &mcs)==1) {
                    
                    diffusion_rate=0.0;
                    
                    
                }
                
                else{
                    
                    
                    
                    
                    
                    neighbours_at_final_position_first_order=
                    free_occupied_detector(i_periodic(i_final_position,&mcs), j_periodic(j_final_position+1,&mcs),&mcs)+
                    free_occupied_detector(i_periodic(i_final_position-1,&mcs), j_periodic(j_final_position+1,&mcs),&mcs)+
                    free_occupied_detector(i_periodic(i_final_position-1,&mcs), j_periodic(j_final_position,&mcs),&mcs)+
                    free_occupied_detector(i_periodic(i_final_position,&mcs), j_periodic(j_final_position-1,&mcs),&mcs)+
                    free_occupied_detector(i_periodic(i_final_position+1,&mcs), j_periodic(j_final_position-1,&mcs),&mcs)+
                    free_occupied_detector(i_periodic(i_final_position+1,&mcs), j_periodic(j_final_position,&mcs),&mcs)
                    -1;
                    
                    // The numbers of neighbours at final position must be reduced by 1  for the energy calculation since the diffusing atom is its own neighbour at final position otherwise
                    
                    
                    
                    neighbours_at_final_position_second_order=
                    free_occupied_detector(i_periodic(i_final_position-1,&mcs), j_periodic(j_final_position+2,&mcs),&mcs)+
                    free_occupied_detector(i_periodic(i_final_position-2,&mcs), j_periodic(j_final_position+1,&mcs),&mcs)+
                    free_occupied_detector(i_periodic(i_final_position-1,&mcs), j_periodic(j_final_position-1,&mcs),&mcs)+
                    free_occupied_detector(i_periodic(i_final_position+1,&mcs), j_periodic(j_final_position-2,&mcs),&mcs)+
                    free_occupied_detector(i_periodic(i_final_position+2,&mcs), j_periodic(j_final_position-1,&mcs),&mcs)+
                    free_occupied_detector(i_periodic(i_final_position+1,&mcs), j_periodic(j_final_position+1,&mcs),&mcs);
                    
                    
                    
                    neighbours_at_final_position_third_order=
                    free_occupied_detector(i_periodic(i_final_position,&mcs), j_periodic(j_final_position+2,&mcs),&mcs)+
                    free_occupied_detector(i_periodic(i_final_position-2,&mcs), j_periodic(j_final_position+2,&mcs),&mcs)+
                    free_occupied_detector(i_periodic(i_final_position-2,&mcs), j_periodic(j_final_position,&mcs),&mcs)+
                    free_occupied_detector(i_periodic(i_final_position,&mcs), j_periodic(j_final_position-2,&mcs),&mcs)+
                    free_occupied_detector(i_periodic(i_final_position+2,&mcs), j_periodic(j_final_position-2,&mcs),&mcs)+
                    free_occupied_detector(i_periodic(i_final_position+2,&mcs), j_periodic(j_final_position,&mcs),&mcs);
                    
                    
                    diffusion_activation_energy_influenced_by_neighbours=(DIFFUSION_ACTIVATION_ENERGY+(neighbours_at_initial_position_first_order*1.*NN_ENERGY)+(neighbours_at_initial_position_second_order*1.*NNN_ENERGY)+(neighbours_at_initial_position_third_order*1.*NNNN_ENERGY));
                    
                    
                    energy_difference_intial_final_position=(neighbours_at_initial_position_first_order-neighbours_at_final_position_first_order)*1.*NN_ENERGY+(neighbours_at_initial_position_second_order-neighbours_at_final_position_second_order)*1.*NNN_ENERGY+(neighbours_at_initial_position_third_order-neighbours_at_final_position_third_order)*1.*NNNN_ENERGY;
                    
                    
                    
                    
                    local_activation_energy=diffusion_activation_energy_influenced_by_neighbours+energy_difference_intial_final_position/BARRIER_DEFORMATION_FACTOR;
                    
                    diffusion_rate=PREEXPFACTOR_DIFFUSION
                    *exp(-(local_activation_energy*ELECTRON_CHARGE)/(K_BOLTZMANN*mcs.temperature));
                    
                    
                    
                    //  Now comes the hindering of on top diffusion
                    
                    on_top_diffusion_i_coordinate=i_periodic(i_initial-1,&mcs);
                    on_top_diffusion_j_coordinate=j_periodic(j_initial+1,&mcs);
                    
                    
                    
                    if(free_occupied_detector(on_top_diffusion_i_coordinate, on_top_diffusion_j_coordinate,&mcs)==1){
                        
                        
                        
                        
                        diffusion_rate=PREEXPFACTOR_DIFFUSION
                        *exp(-(VIA_ON_TOP_DIFFUSION_FACTOR*local_activation_energy*ELECTRON_CHARGE)/(K_BOLTZMANN*mcs.temperature));
                        
                    }
                    
                }
                
                // now write it to the table of adsorbate data
                
                mcs.table_of_adsorbate_data[list_of_line_numbers_for_one_adsorbate[ii]][8]=diffusion_rate;
                
                
                
                
            }
            
            if (diffusion_direction==4) {
                
                // diffusion direction 4
                
                //  i-> i and j-> j-1
                
                
                i_final_position=i_initial;   j_final_position=j_periodic(j_initial-1,&mcs);
                
                if (free_occupied_detector(i_final_position, j_final_position, &mcs)==1) {
                    
                    diffusion_rate=0.0;
                    
                    
                }
                
                else{
                    
                    
                    
                    
                    
                    
                    neighbours_at_final_position_first_order=
                    free_occupied_detector(i_periodic(i_final_position,&mcs), j_periodic(j_final_position+1,&mcs),&mcs)+
                    free_occupied_detector(i_periodic(i_final_position-1,&mcs), j_periodic(j_final_position+1,&mcs),&mcs)+
                    free_occupied_detector(i_periodic(i_final_position-1,&mcs), j_periodic(j_final_position,&mcs),&mcs)+
                    free_occupied_detector(i_periodic(i_final_position,&mcs), j_periodic(j_final_position-1,&mcs),&mcs)+
                    free_occupied_detector(i_periodic(i_final_position+1,&mcs), j_periodic(j_final_position-1,&mcs),&mcs)+
                    free_occupied_detector(i_periodic(i_final_position+1,&mcs), j_periodic(j_final_position,&mcs),&mcs)
                    -1;
                    
                    // The numbers of neighbours at final position must be reduced by 1  for the energy calculation since the diffusing atom is its own neighbour at final position otherwise
                    
                    
                    
                    neighbours_at_final_position_second_order=
                    free_occupied_detector(i_periodic(i_final_position-1,&mcs), j_periodic(j_final_position+2,&mcs),&mcs)+
                    free_occupied_detector(i_periodic(i_final_position-2,&mcs), j_periodic(j_final_position+1,&mcs),&mcs)+
                    free_occupied_detector(i_periodic(i_final_position-1,&mcs), j_periodic(j_final_position-1,&mcs),&mcs)+
                    free_occupied_detector(i_periodic(i_final_position+1,&mcs), j_periodic(j_final_position-2,&mcs),&mcs)+
                    free_occupied_detector(i_periodic(i_final_position+2,&mcs), j_periodic(j_final_position-1,&mcs),&mcs)+
                    free_occupied_detector(i_periodic(i_final_position+1,&mcs), j_periodic(j_final_position+1,&mcs),&mcs);
                    
                    
                    
                    neighbours_at_final_position_third_order=
                    free_occupied_detector(i_periodic(i_final_position,&mcs), j_periodic(j_final_position+2,&mcs),&mcs)+
                    free_occupied_detector(i_periodic(i_final_position-2,&mcs), j_periodic(j_final_position+2,&mcs),&mcs)+
                    free_occupied_detector(i_periodic(i_final_position-2,&mcs), j_periodic(j_final_position,&mcs),&mcs)+
                    free_occupied_detector(i_periodic(i_final_position,&mcs), j_periodic(j_final_position-2,&mcs),&mcs)+
                    free_occupied_detector(i_periodic(i_final_position+2,&mcs), j_periodic(j_final_position-2,&mcs),&mcs)+
                    free_occupied_detector(i_periodic(i_final_position+2,&mcs), j_periodic(j_final_position,&mcs),&mcs);
                    
                    
                    diffusion_activation_energy_influenced_by_neighbours=(DIFFUSION_ACTIVATION_ENERGY+(neighbours_at_initial_position_first_order*1.*NN_ENERGY)+(neighbours_at_initial_position_second_order*1.*NNN_ENERGY)+(neighbours_at_initial_position_third_order*1.*NNNN_ENERGY));
                    
                    
                    energy_difference_intial_final_position=(neighbours_at_initial_position_first_order-neighbours_at_final_position_first_order)*1.*NN_ENERGY+(neighbours_at_initial_position_second_order-neighbours_at_final_position_second_order)*1.*NNN_ENERGY+(neighbours_at_initial_position_third_order-neighbours_at_final_position_third_order)*1.*NNNN_ENERGY;
                    
                    
                    
                    
                    local_activation_energy=diffusion_activation_energy_influenced_by_neighbours+energy_difference_intial_final_position/BARRIER_DEFORMATION_FACTOR;
                    
                    diffusion_rate=PREEXPFACTOR_DIFFUSION
                    *exp(-(local_activation_energy*ELECTRON_CHARGE)/(K_BOLTZMANN*mcs.temperature));
                    
                    
                    //  Now comes the hindering of on top diffusion
                    
                    on_top_diffusion_i_coordinate=i_periodic(i_initial+1,&mcs);
                    on_top_diffusion_j_coordinate=j_periodic(j_initial-1,&mcs);
                    
                    
                    
                    if(free_occupied_detector(on_top_diffusion_i_coordinate, on_top_diffusion_j_coordinate,&mcs)==1){
                        
                        
                        
                        
                        diffusion_rate=PREEXPFACTOR_DIFFUSION
                        *exp(-(VIA_ON_TOP_DIFFUSION_FACTOR*local_activation_energy*ELECTRON_CHARGE)/(K_BOLTZMANN*mcs.temperature));
                        
                    }
                }
                
                
                // now write it to the table of adsorbate data
                
                mcs.table_of_adsorbate_data[list_of_line_numbers_for_one_adsorbate[ii]][8]=diffusion_rate;
                
                
                
            }
            
            if (diffusion_direction==5) {
                
                // diffusion direction 5
                
                //  i-> i+1 and j-> j-1
                
                
                i_final_position=i_periodic(i_initial+1,&mcs);   j_final_position=j_periodic(j_initial-1,&mcs);
                
                if (free_occupied_detector(i_final_position, j_final_position, &mcs)==1) {
                    
                    diffusion_rate=0.0;
                    
                    
                }
                
                else{
                    
                    
                    
                    
                    
                    
                    neighbours_at_final_position_first_order=
                    free_occupied_detector(i_periodic(i_final_position,&mcs), j_periodic(j_final_position+1,&mcs),&mcs)+
                    free_occupied_detector(i_periodic(i_final_position-1,&mcs), j_periodic(j_final_position+1,&mcs),&mcs)+
                    free_occupied_detector(i_periodic(i_final_position-1,&mcs), j_periodic(j_final_position,&mcs),&mcs)+
                    free_occupied_detector(i_periodic(i_final_position,&mcs), j_periodic(j_final_position-1,&mcs),&mcs)+
                    free_occupied_detector(i_periodic(i_final_position+1,&mcs), j_periodic(j_final_position-1,&mcs),&mcs)+
                    free_occupied_detector(i_periodic(i_final_position+1,&mcs), j_periodic(j_final_position,&mcs),&mcs)
                    -1;
                    
                    // The numbers of neighbours at final position must be reduced by 1  for the energy calculation since the diffusing atom is its own neighbour at final position otherwise
                    
                    
                    
                    neighbours_at_final_position_second_order=
                    free_occupied_detector(i_periodic(i_final_position-1,&mcs), j_periodic(j_final_position+2,&mcs),&mcs)+
                    free_occupied_detector(i_periodic(i_final_position-2,&mcs), j_periodic(j_final_position+1,&mcs),&mcs)+
                    free_occupied_detector(i_periodic(i_final_position-1,&mcs), j_periodic(j_final_position-1,&mcs),&mcs)+
                    free_occupied_detector(i_periodic(i_final_position+1,&mcs), j_periodic(j_final_position-2,&mcs),&mcs)+
                    free_occupied_detector(i_periodic(i_final_position+2,&mcs), j_periodic(j_final_position-1,&mcs),&mcs)+
                    free_occupied_detector(i_periodic(i_final_position+1,&mcs), j_periodic(j_final_position+1,&mcs),&mcs);
                    
                    
                    
                    neighbours_at_final_position_third_order=
                    free_occupied_detector(i_periodic(i_final_position,&mcs), j_periodic(j_final_position+2,&mcs),&mcs)+
                    free_occupied_detector(i_periodic(i_final_position-2,&mcs), j_periodic(j_final_position+2,&mcs),&mcs)+
                    free_occupied_detector(i_periodic(i_final_position-2,&mcs), j_periodic(j_final_position,&mcs),&mcs)+
                    free_occupied_detector(i_periodic(i_final_position,&mcs), j_periodic(j_final_position-2,&mcs),&mcs)+
                    free_occupied_detector(i_periodic(i_final_position+2,&mcs), j_periodic(j_final_position-2,&mcs),&mcs)+
                    free_occupied_detector(i_periodic(i_final_position+2,&mcs), j_periodic(j_final_position,&mcs),&mcs);
                    
                    
                    diffusion_activation_energy_influenced_by_neighbours=(DIFFUSION_ACTIVATION_ENERGY+(neighbours_at_initial_position_first_order*1.*NN_ENERGY)+(neighbours_at_initial_position_second_order*1.*NNN_ENERGY)+(neighbours_at_initial_position_third_order*1.*NNNN_ENERGY));
                    
                    
                    energy_difference_intial_final_position=(neighbours_at_initial_position_first_order-neighbours_at_final_position_first_order)*1.*NN_ENERGY+(neighbours_at_initial_position_second_order-neighbours_at_final_position_second_order)*1.*NNN_ENERGY+(neighbours_at_initial_position_third_order-neighbours_at_final_position_third_order)*1.*NNNN_ENERGY;
                    
                    
                    
                    
                    local_activation_energy=diffusion_activation_energy_influenced_by_neighbours+energy_difference_intial_final_position/BARRIER_DEFORMATION_FACTOR;
                    
                    diffusion_rate=PREEXPFACTOR_DIFFUSION
                    *exp(-(local_activation_energy*ELECTRON_CHARGE)/(K_BOLTZMANN*mcs.temperature));
                    
                    
                    
                    //  Now comes the hindering of on top diffusion
                    
                    on_top_diffusion_i_coordinate=i_periodic(i_initial,&mcs);
                    on_top_diffusion_j_coordinate=j_periodic(j_initial-1,&mcs);
                    
                    
                    
                    if(free_occupied_detector(on_top_diffusion_i_coordinate, on_top_diffusion_j_coordinate,&mcs)==1){
                        
                        
                        
                        
                        diffusion_rate=PREEXPFACTOR_DIFFUSION
                        *exp(-(VIA_ON_TOP_DIFFUSION_FACTOR*local_activation_energy*ELECTRON_CHARGE)/(K_BOLTZMANN*mcs.temperature));
                        
                    }
                    
                }
                
                // now write it to the table of adsorbate data
                
                mcs.table_of_adsorbate_data[list_of_line_numbers_for_one_adsorbate[ii]][8]=diffusion_rate;
                
                
            }
            
            if (diffusion_direction==6) {
                
                // diffusion direction 6
                
                //  i-> i+1 and j-> j
                
                
                i_final_position=i_periodic(i_initial+1,&mcs);   j_final_position=j_initial;
                
                if (free_occupied_detector(i_final_position, j_final_position, &mcs)==1) {
                    
                    diffusion_rate=0.0;
                    
                    
                }
                
                else{
                    
                    
                    
                    
                    
                    neighbours_at_final_position_first_order=
                    free_occupied_detector(i_periodic(i_final_position,&mcs), j_periodic(j_final_position+1,&mcs),&mcs)+
                    free_occupied_detector(i_periodic(i_final_position-1,&mcs), j_periodic(j_final_position+1,&mcs),&mcs)+
                    free_occupied_detector(i_periodic(i_final_position-1,&mcs), j_periodic(j_final_position,&mcs),&mcs)+
                    free_occupied_detector(i_periodic(i_final_position,&mcs), j_periodic(j_final_position-1,&mcs),&mcs)+
                    free_occupied_detector(i_periodic(i_final_position+1,&mcs), j_periodic(j_final_position-1,&mcs),&mcs)+
                    free_occupied_detector(i_periodic(i_final_position+1,&mcs), j_periodic(j_final_position,&mcs),&mcs)
                    -1;
                    
                    // The numbers of neighbours at final position must be reduced by 1  for the energy calculation since the diffusing atom is its own neighbour at final position otherwise
                    
                    
                    
                    neighbours_at_final_position_second_order=
                    free_occupied_detector(i_periodic(i_final_position-1,&mcs), j_periodic(j_final_position+2,&mcs),&mcs)+
                    free_occupied_detector(i_periodic(i_final_position-2,&mcs), j_periodic(j_final_position+1,&mcs),&mcs)+
                    free_occupied_detector(i_periodic(i_final_position-1,&mcs), j_periodic(j_final_position-1,&mcs),&mcs)+
                    free_occupied_detector(i_periodic(i_final_position+1,&mcs), j_periodic(j_final_position-2,&mcs),&mcs)+
                    free_occupied_detector(i_periodic(i_final_position+2,&mcs), j_periodic(j_final_position-1,&mcs),&mcs)+
                    free_occupied_detector(i_periodic(i_final_position+1,&mcs), j_periodic(j_final_position+1,&mcs),&mcs);
                    
                    
                    
                    neighbours_at_final_position_third_order=
                    free_occupied_detector(i_periodic(i_final_position,&mcs), j_periodic(j_final_position+2,&mcs),&mcs)+
                    free_occupied_detector(i_periodic(i_final_position-2,&mcs), j_periodic(j_final_position+2,&mcs),&mcs)+
                    free_occupied_detector(i_periodic(i_final_position-2,&mcs), j_periodic(j_final_position,&mcs),&mcs)+
                    free_occupied_detector(i_periodic(i_final_position,&mcs), j_periodic(j_final_position-2,&mcs),&mcs)+
                    free_occupied_detector(i_periodic(i_final_position+2,&mcs), j_periodic(j_final_position-2,&mcs),&mcs)+
                    free_occupied_detector(i_periodic(i_final_position+2,&mcs), j_periodic(j_final_position,&mcs),&mcs);
                    
                    
                    diffusion_activation_energy_influenced_by_neighbours=(DIFFUSION_ACTIVATION_ENERGY+(neighbours_at_initial_position_first_order*1.*NN_ENERGY)+(neighbours_at_initial_position_second_order*1.*NNN_ENERGY)+(neighbours_at_initial_position_third_order*1.*NNNN_ENERGY));
                    
                    
                    energy_difference_intial_final_position=(neighbours_at_initial_position_first_order-neighbours_at_final_position_first_order)*1.*NN_ENERGY+(neighbours_at_initial_position_second_order-neighbours_at_final_position_second_order)*1.*NNN_ENERGY+(neighbours_at_initial_position_third_order-neighbours_at_final_position_third_order)*1.*NNNN_ENERGY;
                    
                    
                    
                    
                    local_activation_energy=diffusion_activation_energy_influenced_by_neighbours+energy_difference_intial_final_position/BARRIER_DEFORMATION_FACTOR;
                    
                    diffusion_rate=PREEXPFACTOR_DIFFUSION
                    *exp(-(local_activation_energy*ELECTRON_CHARGE)/(K_BOLTZMANN*mcs.temperature));
                    
                    
                    //  Now comes the hindering of on top diffusion
                    
                    on_top_diffusion_i_coordinate=i_periodic(i_initial,&mcs);
                    on_top_diffusion_j_coordinate=j_periodic(j_initial+1,&mcs);
                    
                    
                    
                    if(free_occupied_detector(on_top_diffusion_i_coordinate, on_top_diffusion_j_coordinate,&mcs)==1){
                        
                        
                        
                        
                        diffusion_rate=PREEXPFACTOR_DIFFUSION
                        *exp(-(VIA_ON_TOP_DIFFUSION_FACTOR*local_activation_energy*ELECTRON_CHARGE)/(K_BOLTZMANN*mcs.temperature));
                        
                    }
                    
                }
                
                
                // now write it to the table of adsorbate data
                
                mcs.table_of_adsorbate_data[list_of_line_numbers_for_one_adsorbate[ii]][8]=diffusion_rate;
                
                
            }
            
            
        }
        
    }
    
}



void count_atoms_with_less_than_six_neighbours_check_sum_diffrates(struct MCS_STRUCT *ptr_to_mcs_struct)
{
    
    ptr_to_mcs_struct=&mcs;
    
    int i=0;
    int number_of_adsorbates_with_less_than_six_neighbours=0;
    
    double sum_of_diffusion_rates=0.0;
    
    for(i=1;i<=mcs.number_of_adsorbates*6;i++)
    {
        if ( (int) (mcs.table_of_adsorbate_data[i][4] ) < 6 ) {
            
            number_of_adsorbates_with_less_than_six_neighbours+=1;
            
        }
        
        sum_of_diffusion_rates+=mcs.table_of_adsorbate_data[i][8];
        
        
        
        
    }
    
    mcs.number_of_adsorbates_with_less_than_six_neigh= (int)(number_of_adsorbates_with_less_than_six_neighbours/6);
    mcs.sum_of_diffusion_rates=sum_of_diffusion_rates;
    
    
}


void  rewrite_data_for_selected_atoms_after_diffusion(struct MCS_STRUCT *ptr_to_mcs_struct){
    
    ptr_to_mcs_struct=&mcs;
    
    int neighbours_at_initial_position_first_order=0,neighbours_at_initial_position_second_order=0,neighbours_at_initial_position_third_order=0,
    neighbours_at_final_position_first_order=0,
    neighbours_at_final_position_second_order=0,
    neighbours_at_final_position_third_order=0;
    
    
    double diffusion_activation_energy_influenced_by_neighbours=0.0,energy_difference_intial_final_position=0.0,local_activation_energy=0.0,diffusion_rate=0.0;
    
    double sum_of_changes_diffusion_rates=0.0;
    
    double old_value_for_diffusion_rate=0.0;
    
    int diffusion_direction=0,i_selected=0,j_selected=0,i_initial=0,j_initial=0,i_final_position=0,j_final_position=0,on_top_diffusion_i_coordinate=0,on_top_diffusion_j_coordinate=0;
    
    
    int i=0,ii=0,j=0;
    
    int adsorbate_id;
    
    int list_of_line_numbers_for_one_adsorbate[7];
    
    int number_of_entries_for_one_adsorbate=0;
    
    int  old_number_of_neighbours_first_order=0;
    
    
    
    mcs.sorting_and_counting_necessary=false;
    
    // rewrite the lines in the table_of_adsorbate_data where influenced adsorbates by diffusion reside
    
    
    // at first rewrite all the neighbours
    
    for (j=1; j<=mcs.number_of_adsorbates_influenced_by_diffusion; j++) {
        
        // j  is the line index in table of adsorbates influenced by diffusion
        
        adsorbate_id=mcs.table_of_adsorbates_influenced_by_diffusion[j][1];
        
        
        number_of_entries_for_one_adsorbate=0;
        
        
        
        
        // find the 6 line numbers  where the selected atom is in the table_of_adsorbate_data
        
        for (i=1; i<=mcs.number_of_adsorbates*6; i++) {
            
            // i is the line index in table_of_adsorbate_data
            
            
            if (adsorbate_id ==  (int) (mcs.table_of_adsorbate_data[i][1]) ) {
                
                number_of_entries_for_one_adsorbate+=1;
                
                list_of_line_numbers_for_one_adsorbate[number_of_entries_for_one_adsorbate]=i;
                
            }
            
        }
        
        
        
        // give the error message
        
        if (number_of_entries_for_one_adsorbate < 6) {
            
            
            print_adsorbate_data(&mcs);
            
            
            
            
            
            printf("error in rewrite data for selected atoms \nLess than six entries found for one adsorbate \n");
            
            
        }
        
        
        
        
        // diagnosis think about renaming i_selected to i_initial and j in the same way.
        
        
        for (ii=1; ii<=number_of_entries_for_one_adsorbate; ii++) {
            
            // ii is the line index in list of line numbers
            
            // diagnosis think about renaming i_selected to i_initial and j in the same way.
            
            
            if(ii==1){
                
                // take the first line where the adsorbate with the chosen adsorbate id is and rewrite the neighbours
                
                i_selected= (int) (mcs.table_of_adsorbate_data[list_of_line_numbers_for_one_adsorbate[ii]][2] );
                j_selected=  (int) (mcs.table_of_adsorbate_data[list_of_line_numbers_for_one_adsorbate[ii]][3] );
                
                old_number_of_neighbours_first_order= (int) (mcs.table_of_adsorbate_data[list_of_line_numbers_for_one_adsorbate[ii]][4]);
                
                
                
                
                
                neighbours_at_initial_position_first_order=
                free_occupied_detector(i_periodic(i_selected+1,&mcs), j_periodic(j_selected,&mcs),&mcs)+
                free_occupied_detector(i_periodic(i_selected+1,&mcs), j_periodic(j_selected-1,&mcs),&mcs)+
                free_occupied_detector(i_periodic(i_selected,&mcs), j_periodic(j_selected-1,&mcs),&mcs)+
                free_occupied_detector(i_periodic(i_selected-1,&mcs), j_periodic(j_selected,&mcs),&mcs)+
                free_occupied_detector(i_periodic(i_selected-1,&mcs), j_periodic(j_selected+1,&mcs),&mcs)+
                free_occupied_detector(i_periodic(i_selected,&mcs), j_periodic(j_selected+1,&mcs),&mcs);
                
                
                
                neighbours_at_initial_position_second_order=
                free_occupied_detector(i_periodic(i_selected+2,&mcs), j_periodic(j_selected-1,&mcs),&mcs)+
                free_occupied_detector(i_periodic(i_selected+1,&mcs), j_periodic(j_selected-2,&mcs),&mcs)+
                free_occupied_detector(i_periodic(i_selected-1,&mcs), j_periodic(j_selected-1,&mcs),&mcs)+
                free_occupied_detector(i_periodic(i_selected-2,&mcs), j_periodic(j_selected+1,&mcs),&mcs)+
                free_occupied_detector(i_periodic(i_selected-1,&mcs), j_periodic(j_selected+2,&mcs),&mcs)+
                free_occupied_detector(i_periodic(i_selected+1,&mcs), j_periodic(j_selected+1,&mcs),&mcs);
                
                
                neighbours_at_initial_position_third_order=
                free_occupied_detector(i_periodic(i_selected+2,&mcs), j_periodic(j_selected,&mcs),&mcs)+
                free_occupied_detector(i_periodic(i_selected+2,&mcs), j_periodic(j_selected-2,&mcs),&mcs)+
                free_occupied_detector(i_periodic(i_selected,&mcs), j_periodic(j_selected-2,&mcs),&mcs)+
                free_occupied_detector(i_periodic(i_selected-2,&mcs), j_periodic(j_selected,&mcs),&mcs)+
                free_occupied_detector(i_periodic(i_selected-2,&mcs), j_periodic(j_selected+2,&mcs),&mcs)+
                free_occupied_detector(i_periodic(i_selected,&mcs), j_periodic(j_selected+2,&mcs),&mcs);
                
                
                // check now whether whether the number of neighbours in first order has been lower than 6 and is now 6 or whether the number of neighbours was 6 and is now lower than 6. In this case the count and sort procecures should run.
                
                
                if (old_number_of_neighbours_first_order<6 &&  neighbours_at_initial_position_first_order==6  ) {
                    
                    
                    
                    mcs.sorting_and_counting_necessary=true;
                }
                
                if (old_number_of_neighbours_first_order==6 &&  neighbours_at_initial_position_first_order<6  ) {
                    mcs.sorting_and_counting_necessary=true;
                }
                
                
                
                mcs.table_of_adsorbate_data[list_of_line_numbers_for_one_adsorbate[ii]][4]=(double) (neighbours_at_initial_position_first_order);
                mcs.table_of_adsorbate_data[list_of_line_numbers_for_one_adsorbate[ii]][5]=(double) (neighbours_at_initial_position_second_order);
                mcs.table_of_adsorbate_data[list_of_line_numbers_for_one_adsorbate[ii]][6]=(double) (neighbours_at_initial_position_third_order);
                
            }
            
            if(ii>1){
                
                // for ii between 2 and 6 fill the number of neighours  at the other 5 lines
                
                mcs.table_of_adsorbate_data[list_of_line_numbers_for_one_adsorbate[ii]][4]=(double) (neighbours_at_initial_position_first_order);
                mcs.table_of_adsorbate_data[list_of_line_numbers_for_one_adsorbate[ii]][5]=(double) (neighbours_at_initial_position_second_order);
                mcs.table_of_adsorbate_data[list_of_line_numbers_for_one_adsorbate[ii]][6]=(double) (neighbours_at_initial_position_third_order);
                
                
                
            }
            
        }
        
    }
    
    
    // now rewrite the rates
    
    
    for (j=1; j<=mcs.number_of_adsorbates_influenced_by_diffusion; j++) {
        
        
        // j  is the line index in table of adsorbates influenced by diffusion
        
        adsorbate_id=mcs.table_of_adsorbates_influenced_by_diffusion[j][1];
        
        
        number_of_entries_for_one_adsorbate=0;
        
        
        
        // find the 6 line numbers  where the selected atom is in the table_of_adsorbate_data
        
        for (i=1; i<=mcs.number_of_adsorbates*6; i++) {
            
            // i is the line index in table_of_adsorbate_data
            
            
            if (  (int) (mcs.table_of_adsorbate_data[i][1]) == adsorbate_id ) {
                
                number_of_entries_for_one_adsorbate+=1;
                
                list_of_line_numbers_for_one_adsorbate[number_of_entries_for_one_adsorbate]=i;
                
            }
            
        }
        
        
        
        // give the error message
        
        if (number_of_entries_for_one_adsorbate < 6) {
            
            
            
            
            printf("error in rewrite data for selected atoms after diffusion \nLess than six entries found for one adsorbate \n");
            
            
        }
        
        
        
        
        
        
        
        // now write the six diffusion rates for the adsorbate with the coordinates i_initial and j_initial
        
        // for this get the diffusion directions of the atom with the values i_initial, j_initial, adsorbate_index_selected
        
        
        
        // get the diffusion direction
        
        
        for (ii=1; ii<=number_of_entries_for_one_adsorbate; ii++) {
            
            // ii is the line index in list of line numbers
            
            
            if (ii==1) {
                i_initial=(int)( mcs.table_of_adsorbate_data[list_of_line_numbers_for_one_adsorbate[ii]][2]  );
                j_initial=(int)( mcs.table_of_adsorbate_data[list_of_line_numbers_for_one_adsorbate[ii]][3]  );
                
                
            }
            
            diffusion_direction=(int)(round((mcs.table_of_adsorbate_data[list_of_line_numbers_for_one_adsorbate[ii]][7])));
            
            
            //  determine the number of neighbouors at the final position for the diffusion directions 1 to 6
            
            
            if (diffusion_direction==1) {
                
                // diffusion direction 1
                //  i-> i and  j-> j+1
                
                
                i_final_position=i_periodic(i_initial,&mcs);   j_final_position=j_periodic(j_initial+1,&mcs);
                
                if (free_occupied_detector(i_final_position, j_final_position, &mcs)==1) {
                    
                    diffusion_rate=0.0;
                    
                    
                }
                
                else{
                    
                    
                    
                    
                    neighbours_at_final_position_first_order=
                    free_occupied_detector(i_periodic(i_final_position,&mcs), j_periodic(j_final_position+1,&mcs),&mcs)+
                    free_occupied_detector(i_periodic(i_final_position-1,&mcs), j_periodic(j_final_position+1,&mcs),&mcs)+
                    free_occupied_detector(i_periodic(i_final_position-1,&mcs), j_periodic(j_final_position,&mcs),&mcs)+
                    free_occupied_detector(i_periodic(i_final_position,&mcs), j_periodic(j_final_position-1,&mcs),&mcs)+
                    free_occupied_detector(i_periodic(i_final_position+1,&mcs), j_periodic(j_final_position-1,&mcs),&mcs)+
                    free_occupied_detector(i_periodic(i_final_position+1,&mcs), j_periodic(j_final_position,&mcs),&mcs)
                    -1;
                    
                    // The numbers of neighbours at final position must be reduced by 1  for the energy calculation since the diffusing atom is its own neighbour at final position otherwise
                    
                    if (neighbours_at_final_position_first_order==5) {
                        mcs.sorting_and_counting_necessary=true;
                    }
                    
                    neighbours_at_final_position_second_order=
                    free_occupied_detector(i_periodic(i_final_position-1,&mcs), j_periodic(j_final_position+2,&mcs),&mcs)+
                    free_occupied_detector(i_periodic(i_final_position-2,&mcs), j_periodic(j_final_position+1,&mcs),&mcs)+
                    free_occupied_detector(i_periodic(i_final_position-1,&mcs), j_periodic(j_final_position-1,&mcs),&mcs)+
                    free_occupied_detector(i_periodic(i_final_position+1,&mcs), j_periodic(j_final_position-2,&mcs),&mcs)+
                    free_occupied_detector(i_periodic(i_final_position+2,&mcs), j_periodic(j_final_position-1,&mcs),&mcs)+
                    free_occupied_detector(i_periodic(i_final_position+1,&mcs), j_periodic(j_final_position+1,&mcs),&mcs);
                    
                    
                    
                    neighbours_at_final_position_third_order=
                    free_occupied_detector(i_periodic(i_final_position,&mcs), j_periodic(j_final_position+2,&mcs),&mcs)+
                    free_occupied_detector(i_periodic(i_final_position-2,&mcs), j_periodic(j_final_position+2,&mcs),&mcs)+
                    free_occupied_detector(i_periodic(i_final_position-2,&mcs), j_periodic(j_final_position,&mcs),&mcs)+
                    free_occupied_detector(i_periodic(i_final_position,&mcs), j_periodic(j_final_position-2,&mcs),&mcs)+
                    free_occupied_detector(i_periodic(i_final_position+2,&mcs), j_periodic(j_final_position-2,&mcs),&mcs)+
                    free_occupied_detector(i_periodic(i_final_position+2,&mcs), j_periodic(j_final_position,&mcs),&mcs);
                    
                    
                    diffusion_activation_energy_influenced_by_neighbours=(DIFFUSION_ACTIVATION_ENERGY+(neighbours_at_initial_position_first_order*1.*NN_ENERGY)+(neighbours_at_initial_position_second_order*1.*NNN_ENERGY)+(neighbours_at_initial_position_third_order*1.*NNNN_ENERGY));
                    
                    
                    energy_difference_intial_final_position=(neighbours_at_initial_position_first_order-neighbours_at_final_position_first_order)*1.*NN_ENERGY+(neighbours_at_initial_position_second_order-neighbours_at_final_position_second_order)*1.*NNN_ENERGY+(neighbours_at_initial_position_third_order-neighbours_at_final_position_third_order)*1.*NNNN_ENERGY;
                    
                    
                    
                    
                    local_activation_energy=diffusion_activation_energy_influenced_by_neighbours+energy_difference_intial_final_position/BARRIER_DEFORMATION_FACTOR;
                    
                    diffusion_rate=PREEXPFACTOR_DIFFUSION
                    *exp(-(local_activation_energy*ELECTRON_CHARGE)/(K_BOLTZMANN*mcs.temperature));
                    
                    
                    
                    //  Now comes the hindering of on top diffusion
                    
                    on_top_diffusion_i_coordinate=i_periodic(i_initial+1,&mcs);
                    on_top_diffusion_j_coordinate=j_periodic(j_initial,&mcs);
                    
                    
                    
                    if(free_occupied_detector(on_top_diffusion_i_coordinate, on_top_diffusion_j_coordinate,&mcs)==1){
                        
                        
                        
                        
                        diffusion_rate=PREEXPFACTOR_DIFFUSION
                        *exp(-(VIA_ON_TOP_DIFFUSION_FACTOR*local_activation_energy*ELECTRON_CHARGE)/(K_BOLTZMANN*mcs.temperature));
                        
                    }
                }
                
                
                // get the old value and summarize the change
                
                old_value_for_diffusion_rate=mcs.table_of_adsorbate_data[list_of_line_numbers_for_one_adsorbate[ii]][8];
                
                sum_of_changes_diffusion_rates+=diffusion_rate-old_value_for_diffusion_rate;
                
                // now write it to the table of adsorbate data
                
                mcs.table_of_adsorbate_data[list_of_line_numbers_for_one_adsorbate[ii]][8]=diffusion_rate;
                
                
                
            }
            
            if (diffusion_direction==2) {
                
                // diffusion direction 2
                //  i-> i-1 and j-> j+1
                
                
                i_final_position=i_periodic(i_initial-1,&mcs);   j_final_position=j_periodic(j_initial+1,&mcs);
                
                
                if (free_occupied_detector(i_final_position, j_final_position, &mcs)==1) {
                    
                    diffusion_rate=0.0;
                    
                    
                }
                
                else{
                    
                    
                    
                    
                    neighbours_at_final_position_first_order=
                    free_occupied_detector(i_periodic(i_final_position,&mcs), j_periodic(j_final_position+1,&mcs),&mcs)+
                    free_occupied_detector(i_periodic(i_final_position-1,&mcs), j_periodic(j_final_position+1,&mcs),&mcs)+
                    free_occupied_detector(i_periodic(i_final_position-1,&mcs), j_periodic(j_final_position,&mcs),&mcs)+
                    free_occupied_detector(i_periodic(i_final_position,&mcs), j_periodic(j_final_position-1,&mcs),&mcs)+
                    free_occupied_detector(i_periodic(i_final_position+1,&mcs), j_periodic(j_final_position-1,&mcs),&mcs)+
                    free_occupied_detector(i_periodic(i_final_position+1,&mcs), j_periodic(j_final_position,&mcs),&mcs)
                    -1;
                    
                    // The numbers of neighbours at final position must be reduced by 1  for the energy calculation since the diffusing atom is its own neighbour at final position otherwise
                    
                    if (neighbours_at_final_position_first_order==5) {
                        mcs.sorting_and_counting_necessary=true;
                    }
                    
                    neighbours_at_final_position_second_order=
                    free_occupied_detector(i_periodic(i_final_position-1,&mcs), j_periodic(j_final_position+2,&mcs),&mcs)+
                    free_occupied_detector(i_periodic(i_final_position-2,&mcs), j_periodic(j_final_position+1,&mcs),&mcs)+
                    free_occupied_detector(i_periodic(i_final_position-1,&mcs), j_periodic(j_final_position-1,&mcs),&mcs)+
                    free_occupied_detector(i_periodic(i_final_position+1,&mcs), j_periodic(j_final_position-2,&mcs),&mcs)+
                    free_occupied_detector(i_periodic(i_final_position+2,&mcs), j_periodic(j_final_position-1,&mcs),&mcs)+
                    free_occupied_detector(i_periodic(i_final_position+1,&mcs), j_periodic(j_final_position+1,&mcs),&mcs);
                    
                    
                    
                    neighbours_at_final_position_third_order=
                    free_occupied_detector(i_periodic(i_final_position,&mcs), j_periodic(j_final_position+2,&mcs),&mcs)+
                    free_occupied_detector(i_periodic(i_final_position-2,&mcs), j_periodic(j_final_position+2,&mcs),&mcs)+
                    free_occupied_detector(i_periodic(i_final_position-2,&mcs), j_periodic(j_final_position,&mcs),&mcs)+
                    free_occupied_detector(i_periodic(i_final_position,&mcs), j_periodic(j_final_position-2,&mcs),&mcs)+
                    free_occupied_detector(i_periodic(i_final_position+2,&mcs), j_periodic(j_final_position-2,&mcs),&mcs)+
                    free_occupied_detector(i_periodic(i_final_position+2,&mcs), j_periodic(j_final_position,&mcs),&mcs);
                    
                    
                    diffusion_activation_energy_influenced_by_neighbours=(DIFFUSION_ACTIVATION_ENERGY+(neighbours_at_initial_position_first_order*1.*NN_ENERGY)+(neighbours_at_initial_position_second_order*1.*NNN_ENERGY)+(neighbours_at_initial_position_third_order*1.*NNNN_ENERGY));
                    
                    
                    energy_difference_intial_final_position=(neighbours_at_initial_position_first_order-neighbours_at_final_position_first_order)*1.*NN_ENERGY+(neighbours_at_initial_position_second_order-neighbours_at_final_position_second_order)*1.*NNN_ENERGY+(neighbours_at_initial_position_third_order-neighbours_at_final_position_third_order)*1.*NNNN_ENERGY;
                    
                    
                    
                    
                    local_activation_energy=diffusion_activation_energy_influenced_by_neighbours+energy_difference_intial_final_position/BARRIER_DEFORMATION_FACTOR;
                    
                    diffusion_rate=PREEXPFACTOR_DIFFUSION
                    *exp(-(local_activation_energy*ELECTRON_CHARGE)/(K_BOLTZMANN*mcs.temperature));
                    
                    
                    
                    //  Now comes the hindering of on top diffusion
                    
                    on_top_diffusion_i_coordinate=i_periodic(i_initial-1,&mcs);
                    on_top_diffusion_j_coordinate=j_periodic(j_initial,&mcs);
                    
                    
                    
                    if(free_occupied_detector(on_top_diffusion_i_coordinate, on_top_diffusion_j_coordinate,&mcs)==1){
                        
                        
                        
                        
                        diffusion_rate=PREEXPFACTOR_DIFFUSION
                        *exp(-(VIA_ON_TOP_DIFFUSION_FACTOR*local_activation_energy*ELECTRON_CHARGE)/(K_BOLTZMANN*mcs.temperature));
                        
                    }
                    
                }
                
                // get the old value and summarize the change
                
                old_value_for_diffusion_rate=mcs.table_of_adsorbate_data[list_of_line_numbers_for_one_adsorbate[ii]][8];
                
                sum_of_changes_diffusion_rates+=diffusion_rate-old_value_for_diffusion_rate;
                
                // now write it to the table of adsorbate data
                
                mcs.table_of_adsorbate_data[list_of_line_numbers_for_one_adsorbate[ii]][8]=diffusion_rate;
                
                
                
                
                
            }
            
            if (diffusion_direction==3) {
                
                // diffusion direction 3
                //  i-> i-1 and j-> j
                
                
                i_final_position=i_periodic(i_initial-1,&mcs);   j_final_position=j_initial;
                
                if (free_occupied_detector(i_final_position, j_final_position, &mcs)==1) {
                    
                    diffusion_rate=0.0;
                    
                    
                }
                
                else{
                    
                    
                    
                    
                    
                    neighbours_at_final_position_first_order=
                    free_occupied_detector(i_periodic(i_final_position,&mcs), j_periodic(j_final_position+1,&mcs),&mcs)+
                    free_occupied_detector(i_periodic(i_final_position-1,&mcs), j_periodic(j_final_position+1,&mcs),&mcs)+
                    free_occupied_detector(i_periodic(i_final_position-1,&mcs), j_periodic(j_final_position,&mcs),&mcs)+
                    free_occupied_detector(i_periodic(i_final_position,&mcs), j_periodic(j_final_position-1,&mcs),&mcs)+
                    free_occupied_detector(i_periodic(i_final_position+1,&mcs), j_periodic(j_final_position-1,&mcs),&mcs)+
                    free_occupied_detector(i_periodic(i_final_position+1,&mcs), j_periodic(j_final_position,&mcs),&mcs)
                    -1;
                    
                    // The numbers of neighbours at final position must be reduced by 1  for the energy calculation since the diffusing atom is its own neighbour at final position otherwise
                    
                    if (neighbours_at_final_position_first_order==5) {
                        mcs.sorting_and_counting_necessary=true;
                    }
                    
                    neighbours_at_final_position_second_order=
                    free_occupied_detector(i_periodic(i_final_position-1,&mcs), j_periodic(j_final_position+2,&mcs),&mcs)+
                    free_occupied_detector(i_periodic(i_final_position-2,&mcs), j_periodic(j_final_position+1,&mcs),&mcs)+
                    free_occupied_detector(i_periodic(i_final_position-1,&mcs), j_periodic(j_final_position-1,&mcs),&mcs)+
                    free_occupied_detector(i_periodic(i_final_position+1,&mcs), j_periodic(j_final_position-2,&mcs),&mcs)+
                    free_occupied_detector(i_periodic(i_final_position+2,&mcs), j_periodic(j_final_position-1,&mcs),&mcs)+
                    free_occupied_detector(i_periodic(i_final_position+1,&mcs), j_periodic(j_final_position+1,&mcs),&mcs);
                    
                    
                    
                    neighbours_at_final_position_third_order=
                    free_occupied_detector(i_periodic(i_final_position,&mcs), j_periodic(j_final_position+2,&mcs),&mcs)+
                    free_occupied_detector(i_periodic(i_final_position-2,&mcs), j_periodic(j_final_position+2,&mcs),&mcs)+
                    free_occupied_detector(i_periodic(i_final_position-2,&mcs), j_periodic(j_final_position,&mcs),&mcs)+
                    free_occupied_detector(i_periodic(i_final_position,&mcs), j_periodic(j_final_position-2,&mcs),&mcs)+
                    free_occupied_detector(i_periodic(i_final_position+2,&mcs), j_periodic(j_final_position-2,&mcs),&mcs)+
                    free_occupied_detector(i_periodic(i_final_position+2,&mcs), j_periodic(j_final_position,&mcs),&mcs);
                    
                    
                    diffusion_activation_energy_influenced_by_neighbours=(DIFFUSION_ACTIVATION_ENERGY+(neighbours_at_initial_position_first_order*1.*NN_ENERGY)+(neighbours_at_initial_position_second_order*1.*NNN_ENERGY)+(neighbours_at_initial_position_third_order*1.*NNNN_ENERGY));
                    
                    
                    energy_difference_intial_final_position=(neighbours_at_initial_position_first_order-neighbours_at_final_position_first_order)*1.*NN_ENERGY+(neighbours_at_initial_position_second_order-neighbours_at_final_position_second_order)*1.*NNN_ENERGY+(neighbours_at_initial_position_third_order-neighbours_at_final_position_third_order)*1.*NNNN_ENERGY;
                    
                    
                    
                    
                    local_activation_energy=diffusion_activation_energy_influenced_by_neighbours+energy_difference_intial_final_position/BARRIER_DEFORMATION_FACTOR;
                    
                    diffusion_rate=PREEXPFACTOR_DIFFUSION
                    *exp(-(local_activation_energy*ELECTRON_CHARGE)/(K_BOLTZMANN*mcs.temperature));
                    
                    
                    
                    //  Now comes the hindering of on top diffusion
                    
                    on_top_diffusion_i_coordinate=i_periodic(i_initial-1,&mcs);
                    on_top_diffusion_j_coordinate=j_periodic(j_initial+1,&mcs);
                    
                    
                    
                    if(free_occupied_detector(on_top_diffusion_i_coordinate, on_top_diffusion_j_coordinate,&mcs)==1){
                        
                        
                        
                        
                        diffusion_rate=PREEXPFACTOR_DIFFUSION
                        *exp(-(VIA_ON_TOP_DIFFUSION_FACTOR*local_activation_energy*ELECTRON_CHARGE)/(K_BOLTZMANN*mcs.temperature));
                        
                    }
                    
                }
                
                
                // get the old value and summarize the change
                
                old_value_for_diffusion_rate=mcs.table_of_adsorbate_data[list_of_line_numbers_for_one_adsorbate[ii]][8];
                
                sum_of_changes_diffusion_rates+=diffusion_rate-old_value_for_diffusion_rate;
                
                // now write it to the table of adsorbate data
                
                mcs.table_of_adsorbate_data[list_of_line_numbers_for_one_adsorbate[ii]][8]=diffusion_rate;
                
                
                
            }
            
            if (diffusion_direction==4) {
                
                // diffusion direction 4
                
                //  i-> i and j-> j-1
                
                
                i_final_position=i_initial;   j_final_position=j_periodic(j_initial-1,&mcs);
                
                if (free_occupied_detector(i_final_position, j_final_position, &mcs)==1) {
                    
                    diffusion_rate=0.0;
                    
                    
                }
                
                else{
                    
                    
                    neighbours_at_final_position_first_order=
                    free_occupied_detector(i_periodic(i_final_position,&mcs), j_periodic(j_final_position+1,&mcs),&mcs)+
                    free_occupied_detector(i_periodic(i_final_position-1,&mcs), j_periodic(j_final_position+1,&mcs),&mcs)+
                    free_occupied_detector(i_periodic(i_final_position-1,&mcs), j_periodic(j_final_position,&mcs),&mcs)+
                    free_occupied_detector(i_periodic(i_final_position,&mcs), j_periodic(j_final_position-1,&mcs),&mcs)+
                    free_occupied_detector(i_periodic(i_final_position+1,&mcs), j_periodic(j_final_position-1,&mcs),&mcs)+
                    free_occupied_detector(i_periodic(i_final_position+1,&mcs), j_periodic(j_final_position,&mcs),&mcs)
                    -1;
                    
                    // The numbers of neighbours at final position must be reduced by 1  for the energy calculation since the diffusing atom is its own neighbour at final position otherwise
                    
                    if (neighbours_at_final_position_first_order==5) {
                        mcs.sorting_and_counting_necessary=true;
                    }
                    
                    neighbours_at_final_position_second_order=
                    free_occupied_detector(i_periodic(i_final_position-1,&mcs), j_periodic(j_final_position+2,&mcs),&mcs)+
                    free_occupied_detector(i_periodic(i_final_position-2,&mcs), j_periodic(j_final_position+1,&mcs),&mcs)+
                    free_occupied_detector(i_periodic(i_final_position-1,&mcs), j_periodic(j_final_position-1,&mcs),&mcs)+
                    free_occupied_detector(i_periodic(i_final_position+1,&mcs), j_periodic(j_final_position-2,&mcs),&mcs)+
                    free_occupied_detector(i_periodic(i_final_position+2,&mcs), j_periodic(j_final_position-1,&mcs),&mcs)+
                    free_occupied_detector(i_periodic(i_final_position+1,&mcs), j_periodic(j_final_position+1,&mcs),&mcs);
                    
                    
                    
                    neighbours_at_final_position_third_order=
                    free_occupied_detector(i_periodic(i_final_position,&mcs), j_periodic(j_final_position+2,&mcs),&mcs)+
                    free_occupied_detector(i_periodic(i_final_position-2,&mcs), j_periodic(j_final_position+2,&mcs),&mcs)+
                    free_occupied_detector(i_periodic(i_final_position-2,&mcs), j_periodic(j_final_position,&mcs),&mcs)+
                    free_occupied_detector(i_periodic(i_final_position,&mcs), j_periodic(j_final_position-2,&mcs),&mcs)+
                    free_occupied_detector(i_periodic(i_final_position+2,&mcs), j_periodic(j_final_position-2,&mcs),&mcs)+
                    free_occupied_detector(i_periodic(i_final_position+2,&mcs), j_periodic(j_final_position,&mcs),&mcs);
                    
                    
                    diffusion_activation_energy_influenced_by_neighbours=(DIFFUSION_ACTIVATION_ENERGY+(neighbours_at_initial_position_first_order*1.*NN_ENERGY)+(neighbours_at_initial_position_second_order*1.*NNN_ENERGY)+(neighbours_at_initial_position_third_order*1.*NNNN_ENERGY));
                    
                    
                    energy_difference_intial_final_position=(neighbours_at_initial_position_first_order-neighbours_at_final_position_first_order)*1.*NN_ENERGY+(neighbours_at_initial_position_second_order-neighbours_at_final_position_second_order)*1.*NNN_ENERGY+(neighbours_at_initial_position_third_order-neighbours_at_final_position_third_order)*1.*NNNN_ENERGY;
                    
                    
                    
                    
                    local_activation_energy=diffusion_activation_energy_influenced_by_neighbours+energy_difference_intial_final_position/BARRIER_DEFORMATION_FACTOR;
                    
                    diffusion_rate=PREEXPFACTOR_DIFFUSION
                    *exp(-(local_activation_energy*ELECTRON_CHARGE)/(K_BOLTZMANN*mcs.temperature));
                    
                    
                    //  Now comes the hindering of on top diffusion
                    
                    on_top_diffusion_i_coordinate=i_periodic(i_initial+1,&mcs);
                    on_top_diffusion_j_coordinate=j_periodic(j_initial-1,&mcs);
                    
                    
                    
                    if(free_occupied_detector(on_top_diffusion_i_coordinate, on_top_diffusion_j_coordinate,&mcs)==1){
                        
                        
                        
                        
                        diffusion_rate=PREEXPFACTOR_DIFFUSION
                        *exp(-(VIA_ON_TOP_DIFFUSION_FACTOR*local_activation_energy*ELECTRON_CHARGE)/(K_BOLTZMANN*mcs.temperature));
                        
                    }
                }
                
                // get the old value and summarize the change
                
                old_value_for_diffusion_rate=mcs.table_of_adsorbate_data[list_of_line_numbers_for_one_adsorbate[ii]][8];
                
                sum_of_changes_diffusion_rates+=diffusion_rate-old_value_for_diffusion_rate;
                
                // now write it to the table of adsorbate data
                
                mcs.table_of_adsorbate_data[list_of_line_numbers_for_one_adsorbate[ii]][8]=diffusion_rate;
                
            }
            
            if (diffusion_direction==5) {
                
                // diffusion direction 5
                
                //  i-> i+1 and j-> j-1
                
                
                i_final_position=i_periodic(i_initial+1,&mcs);   j_final_position=j_periodic(j_initial-1,&mcs);
                
                if (free_occupied_detector(i_final_position, j_final_position, &mcs)==1) {
                    
                    diffusion_rate=0.0;
                    
                    
                }
                
                else{
                    
                    
                    
                    
                    
                    
                    neighbours_at_final_position_first_order=
                    free_occupied_detector(i_periodic(i_final_position,&mcs), j_periodic(j_final_position+1,&mcs),&mcs)+
                    free_occupied_detector(i_periodic(i_final_position-1,&mcs), j_periodic(j_final_position+1,&mcs),&mcs)+
                    free_occupied_detector(i_periodic(i_final_position-1,&mcs), j_periodic(j_final_position,&mcs),&mcs)+
                    free_occupied_detector(i_periodic(i_final_position,&mcs), j_periodic(j_final_position-1,&mcs),&mcs)+
                    free_occupied_detector(i_periodic(i_final_position+1,&mcs), j_periodic(j_final_position-1,&mcs),&mcs)+
                    free_occupied_detector(i_periodic(i_final_position+1,&mcs), j_periodic(j_final_position,&mcs),&mcs)
                    -1;
                    
                    // The numbers of neighbours at final position must be reduced by 1  for the energy calculation since the diffusing atom is its own neighbour at final position otherwise
                    
                    if (neighbours_at_final_position_first_order==5) {
                        mcs.sorting_and_counting_necessary=true;
                    }
                    
                    
                    neighbours_at_final_position_second_order=
                    free_occupied_detector(i_periodic(i_final_position-1,&mcs), j_periodic(j_final_position+2,&mcs),&mcs)+
                    free_occupied_detector(i_periodic(i_final_position-2,&mcs), j_periodic(j_final_position+1,&mcs),&mcs)+
                    free_occupied_detector(i_periodic(i_final_position-1,&mcs), j_periodic(j_final_position-1,&mcs),&mcs)+
                    free_occupied_detector(i_periodic(i_final_position+1,&mcs), j_periodic(j_final_position-2,&mcs),&mcs)+
                    free_occupied_detector(i_periodic(i_final_position+2,&mcs), j_periodic(j_final_position-1,&mcs),&mcs)+
                    free_occupied_detector(i_periodic(i_final_position+1,&mcs), j_periodic(j_final_position+1,&mcs),&mcs);
                    
                    
                    
                    neighbours_at_final_position_third_order=
                    free_occupied_detector(i_periodic(i_final_position,&mcs), j_periodic(j_final_position+2,&mcs),&mcs)+
                    free_occupied_detector(i_periodic(i_final_position-2,&mcs), j_periodic(j_final_position+2,&mcs),&mcs)+
                    free_occupied_detector(i_periodic(i_final_position-2,&mcs), j_periodic(j_final_position,&mcs),&mcs)+
                    free_occupied_detector(i_periodic(i_final_position,&mcs), j_periodic(j_final_position-2,&mcs),&mcs)+
                    free_occupied_detector(i_periodic(i_final_position+2,&mcs), j_periodic(j_final_position-2,&mcs),&mcs)+
                    free_occupied_detector(i_periodic(i_final_position+2,&mcs), j_periodic(j_final_position,&mcs),&mcs);
                    
                    
                    diffusion_activation_energy_influenced_by_neighbours=(DIFFUSION_ACTIVATION_ENERGY+(neighbours_at_initial_position_first_order*1.*NN_ENERGY)+(neighbours_at_initial_position_second_order*1.*NNN_ENERGY)+(neighbours_at_initial_position_third_order*1.*NNNN_ENERGY));
                    
                    
                    energy_difference_intial_final_position=(neighbours_at_initial_position_first_order-neighbours_at_final_position_first_order)*1.*NN_ENERGY+(neighbours_at_initial_position_second_order-neighbours_at_final_position_second_order)*1.*NNN_ENERGY+(neighbours_at_initial_position_third_order-neighbours_at_final_position_third_order)*1.*NNNN_ENERGY;
                    
                    
                    
                    
                    local_activation_energy=diffusion_activation_energy_influenced_by_neighbours+energy_difference_intial_final_position/BARRIER_DEFORMATION_FACTOR;
                    
                    diffusion_rate=PREEXPFACTOR_DIFFUSION
                    *exp(-(local_activation_energy*ELECTRON_CHARGE)/(K_BOLTZMANN*mcs.temperature));
                    
                    
                    
                    //  Now comes the hindering of on top diffusion
                    
                    on_top_diffusion_i_coordinate=i_periodic(i_initial,&mcs);
                    on_top_diffusion_j_coordinate=j_periodic(j_initial-1,&mcs);
                    
                    
                    
                    if(free_occupied_detector(on_top_diffusion_i_coordinate, on_top_diffusion_j_coordinate,&mcs)==1){
                        
                        
                        
                        
                        diffusion_rate=PREEXPFACTOR_DIFFUSION
                        *exp(-(VIA_ON_TOP_DIFFUSION_FACTOR*local_activation_energy*ELECTRON_CHARGE)/(K_BOLTZMANN*mcs.temperature));
                        
                    }
                    
                }
                
                // get the old value and summarize the change
                
                old_value_for_diffusion_rate=mcs.table_of_adsorbate_data[list_of_line_numbers_for_one_adsorbate[ii]][8];
                
                sum_of_changes_diffusion_rates+=diffusion_rate-old_value_for_diffusion_rate;
                
                // now write it to the table of adsorbate data
                
                mcs.table_of_adsorbate_data[list_of_line_numbers_for_one_adsorbate[ii]][8]=diffusion_rate;
                
            }
            
            if (diffusion_direction==6) {
                
                // diffusion direction 6
                
                //  i-> i+1 and j-> j
                
                
                i_final_position=i_periodic(i_initial+1,&mcs);   j_final_position=j_initial;
                
                if (free_occupied_detector(i_final_position, j_final_position, &mcs)==1) {
                    
                    diffusion_rate=0.0;
                    
                    
                }
                
                else{
                    
                    
                    
                    
                    
                    neighbours_at_final_position_first_order=
                    free_occupied_detector(i_periodic(i_final_position,&mcs), j_periodic(j_final_position+1,&mcs),&mcs)+
                    free_occupied_detector(i_periodic(i_final_position-1,&mcs), j_periodic(j_final_position+1,&mcs),&mcs)+
                    free_occupied_detector(i_periodic(i_final_position-1,&mcs), j_periodic(j_final_position,&mcs),&mcs)+
                    free_occupied_detector(i_periodic(i_final_position,&mcs), j_periodic(j_final_position-1,&mcs),&mcs)+
                    free_occupied_detector(i_periodic(i_final_position+1,&mcs), j_periodic(j_final_position-1,&mcs),&mcs)+
                    free_occupied_detector(i_periodic(i_final_position+1,&mcs), j_periodic(j_final_position,&mcs),&mcs)
                    -1;
                    
                    // The numbers of neighbours at final position must be reduced by 1  for the energy calculation since the diffusing atom is its own neighbour at final position otherwise
                    
                    if (neighbours_at_final_position_first_order==5) {
                        mcs.sorting_and_counting_necessary=true;
                    }
                    
                    
                    neighbours_at_final_position_second_order=
                    free_occupied_detector(i_periodic(i_final_position-1,&mcs), j_periodic(j_final_position+2,&mcs),&mcs)+
                    free_occupied_detector(i_periodic(i_final_position-2,&mcs), j_periodic(j_final_position+1,&mcs),&mcs)+
                    free_occupied_detector(i_periodic(i_final_position-1,&mcs), j_periodic(j_final_position-1,&mcs),&mcs)+
                    free_occupied_detector(i_periodic(i_final_position+1,&mcs), j_periodic(j_final_position-2,&mcs),&mcs)+
                    free_occupied_detector(i_periodic(i_final_position+2,&mcs), j_periodic(j_final_position-1,&mcs),&mcs)+
                    free_occupied_detector(i_periodic(i_final_position+1,&mcs), j_periodic(j_final_position+1,&mcs),&mcs);
                    
                    
                    
                    neighbours_at_final_position_third_order=
                    free_occupied_detector(i_periodic(i_final_position,&mcs), j_periodic(j_final_position+2,&mcs),&mcs)+
                    free_occupied_detector(i_periodic(i_final_position-2,&mcs), j_periodic(j_final_position+2,&mcs),&mcs)+
                    free_occupied_detector(i_periodic(i_final_position-2,&mcs), j_periodic(j_final_position,&mcs),&mcs)+
                    free_occupied_detector(i_periodic(i_final_position,&mcs), j_periodic(j_final_position-2,&mcs),&mcs)+
                    free_occupied_detector(i_periodic(i_final_position+2,&mcs), j_periodic(j_final_position-2,&mcs),&mcs)+
                    free_occupied_detector(i_periodic(i_final_position+2,&mcs), j_periodic(j_final_position,&mcs),&mcs);
                    
                    
                    diffusion_activation_energy_influenced_by_neighbours=(DIFFUSION_ACTIVATION_ENERGY+(neighbours_at_initial_position_first_order*1.*NN_ENERGY)+(neighbours_at_initial_position_second_order*1.*NNN_ENERGY)+(neighbours_at_initial_position_third_order*1.*NNNN_ENERGY));
                    
                    
                    energy_difference_intial_final_position=(neighbours_at_initial_position_first_order-neighbours_at_final_position_first_order)*1.*NN_ENERGY+(neighbours_at_initial_position_second_order-neighbours_at_final_position_second_order)*1.*NNN_ENERGY+(neighbours_at_initial_position_third_order-neighbours_at_final_position_third_order)*1.*NNNN_ENERGY;
                    
                    
                    
                    
                    local_activation_energy=diffusion_activation_energy_influenced_by_neighbours+energy_difference_intial_final_position/BARRIER_DEFORMATION_FACTOR;
                    
                    diffusion_rate=PREEXPFACTOR_DIFFUSION
                    *exp(-(local_activation_energy*ELECTRON_CHARGE)/(K_BOLTZMANN*mcs.temperature));
                    
                    
                    //  Now comes the hindering of on top diffusion
                    
                    on_top_diffusion_i_coordinate=i_periodic(i_initial,&mcs);
                    on_top_diffusion_j_coordinate=j_periodic(j_initial+1,&mcs);
                    
                    
                    
                    if(free_occupied_detector(on_top_diffusion_i_coordinate, on_top_diffusion_j_coordinate,&mcs)==1){
                        
                        
                        
                        
                        diffusion_rate=PREEXPFACTOR_DIFFUSION
                        *exp(-(VIA_ON_TOP_DIFFUSION_FACTOR*local_activation_energy*ELECTRON_CHARGE)/(K_BOLTZMANN*mcs.temperature));
                        
                    }
                    
                }
                // get the old value and summarize the change
                
                old_value_for_diffusion_rate=mcs.table_of_adsorbate_data[list_of_line_numbers_for_one_adsorbate[ii]][8];
                
                sum_of_changes_diffusion_rates+=diffusion_rate-old_value_for_diffusion_rate;
                
                // now write it to the table of adsorbate data
                
                mcs.table_of_adsorbate_data[list_of_line_numbers_for_one_adsorbate[ii]][8]=diffusion_rate;
                
            }
        }
        
    }
    
    
    
    // change  the sum of diffusion_rates accordingly
    
    mcs.sum_of_diffusion_rates=mcs.sum_of_diffusion_rates+sum_of_changes_diffusion_rates;
    
    
    
    
}


void mc_step (struct MCS_STRUCT *ptr_to_mcs_struct)
{
    
    
    ptr_to_mcs_struct=&mcs;
    
    double random_number_value=random_number();
    
    mcs.random_number=random_number_value;
    
    
    int i=0;
    
    
    // check the necessity of an adsorption event
    
    
    
    if (mcs.number_of_adsorbates==0) {
        
        perform_adsorption(&mcs);
        
        mcs.number_of_adsorbates_with_less_than_six_neigh=mcs.number_of_adsorbates;
        
        rewrite_all_neighbours_and_rates_after_adsorption(&mcs);
    }
    
    
    
    if (mcs.diagnostics_tools==true) {
        
        //     data_output_eps_ad_id(&mcs);
        
        //    data_output_csv(&mcs);
    }
    
    else{
        
        //  data_output_eps(&mcs);
        
        //    data_output_svg(&mcs);
        
        //    data_output_animated_svg(&mcs);
        
        
        
        
        
    }
    
    
    
    
    mcs.process_counter[8]+=1;
    
    
    
    
    
    
    
    
    
    if (mcs.mc_time/mcs.mean_time_between_two_adsorption_events - (double)(mcs.number_of_adsorbates)>1 && mcs.growth_finished==false) {
        
        perform_adsorption(&mcs);
        
        mcs. number_of_adsorbates_with_less_than_six_neigh=mcs.number_of_adsorbates;
        
        rewrite_all_neighbours_and_rates_after_adsorption(&mcs);
        
        
        qsort(mcs.table_of_adsorbate_data+1,mcs.number_of_adsorbates*6,sizeof(mcs.table_of_adsorbate_data[0]),compare_neighbours_in_table_of_adsorbate_data);
        
        //  qsort(mcs.table_of_adsorbate_data+1,mcs.number_of_adsorbates_with_less_than_six_neigh*6,sizeof(mcs.table_of_adsorbate_data[0]),compare_rates_in_table_of_adsorbate_data);
        
        count_atoms_with_less_than_six_neighbours_check_sum_diffrates(&mcs);
        
        
        mcs.process_counter[8]+=1;
        // increase the rewrite counter by 1
        
        
    }
    
    
    
    
    else  {
        
        //  mcs.number_of_adsorbates_with_less_than_six_neigh=mcs.number_of_adsorbates;
        
        perform_diffusion(&mcs);
        
        
        
        
        
        if (mcs.rewrite_data_for_selected_atoms_necessary==true) {
            
            
            rewrite_data_for_selected_atoms_after_diffusion(&mcs);
            
            mcs.process_counter[8]+=1;
            
        }
        
        
        
        
        if (mcs.sorting_and_counting_necessary==true) {
            
            
            
            qsort(mcs.table_of_adsorbate_data+1,mcs.number_of_adsorbates*6,sizeof(mcs.table_of_adsorbate_data[0]),compare_neighbours_in_table_of_adsorbate_data);
            
            
            count_atoms_with_less_than_six_neighbours_check_sum_diffrates(&mcs);
            
            //   if you want to play around with sorting the adsorbate table in order of the rates you can switch it on here. Be cautious ! Only for people wanting to learn some programming
            //  qsort(mcs.table_of_adsorbate_data+1,mcs.number_of_adsorbates_with_less_than_six_neigh*6,sizeof(mcs.table_of_adsorbate_data[0]),compare_rates_in_table_of_adsorbate_data);
            
            
            //     printf(" A sorting event was performed at mc time %g with %d number of adsorbates with %d number of adsorbates with less than six neigh\n\n",mcs.mc_time,mcs.number_of_adsorbates,mcs.number_of_adsorbates_with_less_than_six_neigh);
            
            
            mcs.process_counter[9]+=1;
            
            // increase the sorting counter by 1
            
            
            
        }
        
        
        
        
        
        
    }
    
    
    
    
    
    mcs.time_increment=1/(mcs.sum_of_adsorption_rates+mcs.sum_of_diffusion_rates);
    
    
    
    if (mcs.number_mc_step==1&& NUMBER_OF_PREADSORBED_ADSORBATES==0) {
        mcs.mc_time=mcs.mean_time_between_two_adsorption_events*1.999999;
        // this is to adjust the time scale after the first adsorption.
        // Simply adding the time increment is wrong since the time increment for the next process is a diffusion process and before this monte carlo step no rates existed at all since the surface was empty.
        
    }
    
    
    
    
    
    if (mcs.number_mc_step>1) {
        mcs.mc_time=mcs.mc_time+mcs.time_increment;
        
    }
    
    
    
    if (mcs.number_mc_step%500000==0||mcs.number_mc_step==10) {
        
        
        
        printf("mc_step[/1000] \tno of adsorbates \tMC_time \t\trewrites of adsorbate table \tsorts of adsorbate table\n");
        
        printf("%09ld \t\t%4d  \t\t%5.4g \t%11d \t\t\t\t\t\t%3d \n",mcs.number_mc_step/1000,mcs.number_of_adsorbates,mcs.mc_time,mcs.process_counter[8],mcs.process_counter[9] );
        
        check_sum_of_diffusion_rates(&mcs);
        
        
        
        
        
    }
    
    for (i=mcs.line_in_experiment_monitor_file; i<=mcs.number_of_eps_files_and_lines_in_experiment_monitor_file ;i++) {
        
        if (mcs.mc_time>mcs.list_mc_times_monitored[i] ) {
            
            
            
            
            mcs.write_adsorbate_matrix=true;
            mcs.line_in_experiment_monitor_file+=1;
            
            
            
            
            
            
            
        }
        
        
        
        
    }
    
    
    
    
}


int main(void)
{
    
    
    fprintf(stdout,"\n\n\n  %d  \n\n\n ",RAND_MAX  );
    
    struct MCS_STRUCT *ptr_to_mcs_struct;
    ptr_to_mcs_struct=&mcs;
    
    
    mcs.diagnostics_tools=DIAGNOSTICS_TOOLS;
    
    
    mcs.start_clock_of_mcs=clock();
    
    
    char temp_string[4]="";
    
    sprintf(temp_string,"%d",TEMPERATURE_DURING_GROWTH);
    
    char fname_experiment_monitor[200]="";
    
    strcat(fname_experiment_monitor,DATA_DIRECTORY);
    
    strcat(fname_experiment_monitor,"experiment_monitor_");
    
    strcat(fname_experiment_monitor,temp_string);
    
    strcat(fname_experiment_monitor,"K.csv");
    
    mcs.fname_experiment_monitor=fname_experiment_monitor;
    
    mcs.number_of_adsorbates=0;
    
    mcs.maximum_number_of_adsorbates=(int) (FINAL_COVERAGE*NUMBER_OF_ROWS*NUMBER_OF_COLUMNS);
    
    
    // the if condition ensures that table is not empty for small matrices
    if (mcs.maximum_number_of_adsorbates<1) {
        mcs.maximum_number_of_adsorbates=1;
    }
    
    
    mcs.mean_time_between_two_adsorption_events=1/(DEPOSITION_RATE*(NUMBER_OF_ROWS*NUMBER_OF_COLUMNS)) ;
    
    
    mcs. number_of_adsorbates_with_less_than_six_neigh=0;
    
    
    mcs.number_of_adsorbates_influenced_by_diffusion=0;
    
    
    mcs.rewrite_counter=0;
    
    mcs.lines_in_history_of_adsorbates_positions=0;
    
    
    long int i,number_mc_step;
    
    
    
    // initialize the table of adsorbate data
    
    // mcs.number_of_adsorbates was initialized as zero above and not changed here
    
    // Allocate the rows
    
    mcs.table_of_adsorbate_data=(double **)calloc((mcs.maximum_number_of_adsorbates*6+1),sizeof(double *));
    
    // Allocate the columns
    
    for(i=0;i<=mcs.maximum_number_of_adsorbates*6;i++){
        
        mcs.table_of_adsorbate_data[i]=(double *)calloc((NUMBER_OF_COLUMNS_IN_TABLE_OF_ADSORBATE_DATA+1),sizeof(double) );
    }
    
    
    
    
    // initialize  the table of adsorbates influenced by diffusion
    
    mcs.table_of_adsorbates_influenced_by_diffusion=(int **)(calloc(MAXIMUM_NUMBER_OF_ADSORBATES_INFLUENCED_BY_DIFFUSION+1, sizeof(int *)));
    
    
    
    for (i=0; i<=MAXIMUM_NUMBER_OF_ADSORBATES_INFLUENCED_BY_DIFFUSION; i++) {
        mcs.table_of_adsorbates_influenced_by_diffusion[i]=(int *) calloc(NUMBER_OF_COLUMNS_IN_TABLE_OF_ADSORBATES_INFLUENCED_BY_DIFFUSION+1,sizeof(int));
    }
    
    
    
    
    
    // initalize the swap line for the table of diffusion rates.
    mcs.swap_line_for_table_of_adsorbate_data = (double **) calloc(1, sizeof(double *));
    if (mcs.swap_line_for_table_of_adsorbate_data == NULL) {
        return 1;
    }
    mcs.swap_line_for_table_of_adsorbate_data[0] = (double *) calloc(NUMBER_OF_COLUMNS_IN_TABLE_OF_ADSORBATE_DATA+1, sizeof(double));
    if (mcs.swap_line_for_table_of_adsorbate_data[0] == NULL) {
        free(mcs.swap_line_for_table_of_adsorbate_data);
        return 1;
    }
    
    double predicted_duration_growth=(double)(FINAL_COVERAGE)/(DEPOSITION_RATE);
    
    mcs.duration_of_growth=predicted_duration_growth;
    
    double heating_rate=((TEMPERATURE_DURING_RIPENING-TEMPERATURE_DURING_GROWTH)*1.)/(DURATION_OF_HEATING_PROCESS*1.);
    
    double delta_t_output=1./((double) (FRAME_RATE));
    
    
    mcs.duration_of_experiment=mcs.duration_of_growth+DURATION_OF_HEATING_PROCESS+DURATION_OF_RIPENING_AT_CONSTANT_TEMPERATURE;
    
    mcs.number_of_eps_files_and_lines_in_experiment_monitor_file=(int)((predicted_duration_growth+DURATION_OF_HEATING_PROCESS+DURATION_OF_RIPENING_AT_CONSTANT_TEMPERATURE)/delta_t_output)+1;
    
    
    clock_t clock_measure = 0;
    
    // initialize the array for the the time evolution file
    
    mcs.lines_in_experiment_monitor_file=1;
    
    double time_since_last_output=0.0;
    
    double time_per_mc_step_per_adsorbate=0.0;
    
    
    mcs.history_of_adsorbate_positions=(int **) (calloc(1, sizeof(int *)));
    for (i=0; i<=mcs.lines_in_history_of_adsorbates_positions; i++) {
        mcs.history_of_adsorbate_positions[i]=(int *)calloc(NUMBER_OF_COLUMNS_IN_HISTORY_OF_ADSORBATE_POSITIONS+1,sizeof(int));
    }
    
    
    
    mcs.experiment_monitor=(double **)(calloc(mcs.lines_in_experiment_monitor_file+1, sizeof(double *)));
    for (i=0; i<=mcs.lines_in_experiment_monitor_file; i++) {
        mcs.experiment_monitor[i]=(double *) calloc(NUMBER_OF_COLUMNS_IN_EXPERIMENT_MONITOR_FILE+1,sizeof(double));
        
    }
    
    
    // initalize the process conter
    
    mcs.process_counter=(int *)calloc(NUMBER_OF_INTEGER_VALUES_IN_PROCESS_COUNTER+1, sizeof(double));
    // diffusion direction 1, 2, 3, 4, 5, 6, adsorption, rewrites of single rates , sorting events
    
    
    
    
    mcs.list_mc_times_monitored=calloc(mcs.number_of_eps_files_and_lines_in_experiment_monitor_file+1,sizeof(double));
    
    // the growth regime
    for (i=1; i<=mcs.number_of_eps_files_and_lines_in_experiment_monitor_file; i++) {
        mcs.list_mc_times_monitored[i]=i*delta_t_output;
    }
    
    
    
    
    printf(" EPS Figure of the asorbate structure\nwill be shown at the following Monte Carlo times:\n");
    
    
    for (i=1; i<mcs.number_of_eps_files_and_lines_in_experiment_monitor_file; i++) {
        printf("file no. %3ld at  %g sec\n",i,mcs.list_mc_times_monitored[i]);
        
    }
    
    
    double actual_temperature=TEMPERATURE_DURING_GROWTH;
    
    
    mcs.temperature=actual_temperature;
    mcs.deposition_rate=DEPOSITION_RATE;
    mcs.sum_of_adsorption_rates=(double) (NUMBER_OF_COLUMNS*NUMBER_OF_ROWS)*DEPOSITION_RATE;
    mcs.number_mc_step=0; mcs.mc_time=0.0;
    mcs.rewrite_data_for_selected_atoms_necessary=false;
    mcs. number_of_adsorbates_with_less_than_six_neigh=0;
    mcs.mc_step_last_output=0;
    mcs.clock_last_output=0;
    
    
    
    // the line below  should be set active if you want to work with different random numbers every time you run the programm.
    srand((unsigned int) time(NULL));
    // if you want to work with always the same set of random numbers, place an // in front of srand(.....
    
    
    // initialize the time evolution file
    
    mcs.experiment_monitor[1][1]=0.0;
    mcs.experiment_monitor[1][2]=mcs.temperature;
    mcs.experiment_monitor[1][3]=0.;
    mcs.experiment_monitor[1][4]=0.;
    mcs.experiment_monitor[1][5]=0.;
    mcs.experiment_monitor[1][6]=0.;
    mcs.experiment_monitor[1][7]=0.;
    mcs.experiment_monitor[1][8]=0.;
    mcs.experiment_monitor[1][9]=0.;
    mcs.experiment_monitor[1][10]=0.;
    mcs.experiment_monitor[1][11]=0.;
    mcs.experiment_monitor[1][12]=0.;
    mcs.experiment_monitor[1][13]=0.;
    mcs.experiment_monitor[1][14]=0.;
    mcs.experiment_monitor[1][15]=0.;
    
    
    fprintf(stdout,"\n\n\nMatrix size is %i * %i\n\n",NUMBER_OF_COLUMNS,NUMBER_OF_ROWS);
    
    
    
    mcs.single_adsorbate_diffusion_time=1/(PREEXPFACTOR_DIFFUSION*exp(-ELECTRON_CHARGE*DIFFUSION_ACTIVATION_ENERGY/(K_BOLTZMANN*TEMPERATURE_DURING_GROWTH)));
    
    
    fprintf(stdout,"\n\nSingle adsorbate diffusion time is %g \n\n",mcs.single_adsorbate_diffusion_time );
    
    
    fprintf(stdout,"\n\nSingle adsorbate diffusion rate is %g \n\n",1./mcs.single_adsorbate_diffusion_time );
    
    printf("\n\nMean Monte Carlo time between two adsorption events is %g\n\n",mcs.mean_time_between_two_adsorption_events );
    
    
    
    
    if(NUMBER_OF_PREADSORBED_ADSORBATES>0)
    {
        
        for(i=1;i<=NUMBER_OF_PREADSORBED_ADSORBATES;i++)
        {
            perform_adsorption(&mcs);
            
        }
        
        mcs.mc_time=NUMBER_OF_PREADSORBED_ADSORBATES*mcs.mean_time_between_two_adsorption_events;
        
        rewrite_all_neighbours_and_rates_after_adsorption(&mcs);
        
        qsort(mcs.table_of_adsorbate_data+1,mcs.number_of_adsorbates*6,sizeof(mcs.table_of_adsorbate_data[0]),compare_neighbours_in_table_of_adsorbate_data);
        
        
        // you can also choose to sort the table of adsorbate data with descending order in diffusion rates. This does not have any effect on chemistry, only interesiting for software guys.
        //   qsort(mcs.table_of_adsorbate_data+1,mcs.number_of_adsorbates*6,sizeof(mcs.table_of_adsorbate_data[0]),compare_rates_in_table_of_adsorbate_data);
        
        
        mcs.number_of_adsorbates_with_less_than_six_neigh=mcs.number_of_adsorbates;
        
        
        printf("preparation finished \n");
        
        
    }
    
    
    for(number_mc_step=1; number_mc_step<=1e11 && mcs.mc_time<=mcs.duration_of_experiment; number_mc_step++) {
        
        mcs.number_mc_step=number_mc_step;
        mcs.write_adsorbate_matrix=false;
        
        
        
        if (mcs.number_of_adsorbates<mcs.maximum_number_of_adsorbates) {
            mcs.temperature=TEMPERATURE_DURING_GROWTH;
            mcs.deposition_rate=DEPOSITION_RATE;
            
        }
        
        
        if(mcs.number_of_adsorbates==mcs.maximum_number_of_adsorbates && mcs.temperature<TEMPERATURE_DURING_RIPENING) {
            
            
            
            
            mcs.temperature=TEMPERATURE_DURING_GROWTH+(mcs.mc_time-mcs.duration_of_growth)*heating_rate;
            
            mcs.deposition_rate=0.0;
            
        }
        
        
        
        if(mcs.number_of_adsorbates>=mcs.maximum_number_of_adsorbates && mcs.mc_time>mcs.duration_of_growth+DURATION_OF_HEATING_PROCESS) {
            
            mcs.temperature=TEMPERATURE_DURING_RIPENING;
            
            mcs.deposition_rate=0.0;
            
        }
        
        
        
        //  qsort(mcs.table_of_adsorbate_data+1,mcs.number_of_adsorbates*6,sizeof(mcs.table_of_adsorbate_data[0]),compare_neighbours_in_table_of_adsorbate_data);
        
        
        //  count_atoms_with_less_than_six_neighbours_check_sum_diffrates(&mcs);
        
        
        
        
        mc_step(&mcs);
        
        
        
        if (mcs.write_adsorbate_matrix==true) {
            
            data_output_experiment_monitor_file(&mcs);
            
            rewrite_history_of_adsorbates(&mcs);
            
            //		print_adsorbate_data(&mcs);
            
            //	print_history_of_adsorbate_data(&mcs);
            
            
            
            
            
            if (mcs.diagnostics_tools==true) {
                
                data_output_svg_ad_id(&mcs);
                
            //    data_output_csv(&mcs);
                
                
            }
            
            else {
                
                //   data_output_eps(&mcs);
                
                
                data_output_svg_ad_id(&mcs);
                
                
                //  data_output_animated_svg(&mcs);
                
                
            }
            
            
            
            
            
            clock_measure=clock();
            
            mcs.cpu_time=(double) (clock_measure-mcs.start_clock_of_mcs)/CLOCKS_PER_SEC*1. ;
            
            
            time_since_last_output=(double) (clock_measure-mcs.clock_last_output) /(double) (CLOCKS_PER_SEC);
            
            time_per_mc_step_per_adsorbate=time_since_last_output/(number_mc_step-mcs.mc_step_last_output)/mcs.number_of_adsorbates;
            
            mcs.time_per_step_per_adsorbate=time_per_mc_step_per_adsorbate;
            
            mcs.clock_last_output=clock_measure;
            mcs.mc_step_last_output=number_mc_step;
            
            
            if (mcs.number_of_adsorbates<mcs.maximum_number_of_adsorbates) {
                
                clock_measure=clock();
                
                
                
                fprintf(stdout,"In growth regime (Period I) with constant temperature %g number of atoms %d \nat monte carlo time %07.04g sec \nat CPU Time %07.04g sec \ntime since last output %07.04g sec \nNumber of atoms with six neighbours is %d\n",mcs.temperature,mcs.number_of_adsorbates,mcs.mc_time,mcs.cpu_time,time_since_last_output,mcs.number_of_adsorbates-mcs.number_of_adsorbates_with_less_than_six_neigh);
                
                
                
                
                
                fprintf(stdout,"mc step %07ld \ndeposition rate is %g\n\n\n",number_mc_step,(double) (mcs.number_of_adsorbates)/(NUMBER_OF_COLUMNS*NUMBER_OF_ROWS*1.)/mcs.mc_time);
                
                //  print_adsorbate_data(&mcs);
                
                
                
                
            }
            
            
            
            if (mcs.number_of_adsorbates>=mcs.maximum_number_of_adsorbates&& mcs.mc_time<=mcs.duration_of_growth+DURATION_OF_HEATING_PROCESS) {
                
                clock_measure=clock();
                
                
                
                
                fprintf(stdout,"In heating regime (Period II) with variable temperature %g number of atoms %d \nat monte carlo time %07.04g sec \nat CPU Time %07.04g sec \ntime since last output %07.04g sec \nNumber of atoms with six neighbours is %d\n",mcs.temperature,mcs.number_of_adsorbates,mcs.mc_time,mcs.cpu_time,time_since_last_output,mcs.number_of_adsorbates-mcs.number_of_adsorbates_with_less_than_six_neigh);
                
                
                fprintf(stdout,"mc step %04.2e \n\n",(double)(number_mc_step));
                
                
            }
            
            
            
            if (mcs.number_of_adsorbates>=mcs.maximum_number_of_adsorbates && mcs.mc_time>mcs.duration_of_growth+DURATION_OF_HEATING_PROCESS ) {
                
                
                clock_measure=clock();
                
                
                
                fprintf(stdout,"In constant temperature regime (Period III) with again constant temperature %g number of atoms %d \nat monte carlo time %07.04g sec \nat CPU Time %07.04g sec \ntime since last output %07.04g sec \nNumber of atoms with six neighbours is %d\n",mcs.temperature,mcs.number_of_adsorbates,mcs.mc_time,mcs.cpu_time,time_since_last_output,mcs.number_of_adsorbates-mcs.number_of_adsorbates_with_less_than_six_neigh);
                
                fprintf(stdout,"mc step %04.2e \n\n",(double)(number_mc_step));
                
                
                
            }
            
            
            
        }
        
        
        
        mcs.write_adsorbate_matrix=false;
        
    }
    
    
    
    // at the end don't forget to free all the memmory once allocated
    
    for(i=0;i<=mcs.maximum_number_of_adsorbates*6;i++){
        
        free(mcs.table_of_adsorbate_data[i]);
    }
    
    free(mcs.table_of_adsorbate_data);
    
    
    for (i=0; i<=MAXIMUM_NUMBER_OF_ADSORBATES_INFLUENCED_BY_DIFFUSION; i++) {
        free(mcs.table_of_adsorbates_influenced_by_diffusion[i]);
    }
    
    free(mcs.table_of_adsorbates_influenced_by_diffusion);
    
    if (mcs.swap_line_for_table_of_adsorbate_data != NULL) {
        if (mcs.swap_line_for_table_of_adsorbate_data[0] != NULL) {
            free(mcs.swap_line_for_table_of_adsorbate_data[0]);
        }
        free(mcs.swap_line_for_table_of_adsorbate_data);
        mcs.swap_line_for_table_of_adsorbate_data = NULL;
    }
    
    for (i=0; i<=mcs.lines_in_experiment_monitor_file; i++) {
        free(mcs.experiment_monitor[i]);}
    
    free(mcs.experiment_monitor);
    
    free(mcs.process_counter);
    
    free(mcs.list_mc_times_monitored);
    
    
    
    return 0;
    
    
    
    
}






