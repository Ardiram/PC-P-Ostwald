//
//  random_walk.c
//  nano_growth_ripen
//
//  Created by diesing on 27/10/2016.
//  if you need any help with this code, please send an email to
// detlef.diesing@uni-due.de



#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <string.h>
#include <stdbool.h>


// this is now for mac os

#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>


# define NUMBER_OF_COLUMNS 350  // surface size in j (horizontal) direction, the default value taken in the Journal of Chemical Education article is 250
                                // value must be larger than 10

# define NUMBER_OF_ROWS 350     // surface size in i (vertical) direction , the default value taken in the Journal of Chemical Education article is 250
                                // value must be larger than 10


    //  Remark: The eps output files of the adsorbate structure are not optimized for NUMBER_OF_COLUMNS not equal to  NUMBER_OF_ROWS but you can play with it.

# define UPPER_LIMIT_FOR_SUBSTRATE_DISPLAY  200 // this is to limit the size of the eps output files

    // if UPPER_LIMIT_FOR_SUBSTRATE_DISPLAY is larger than NUMBER_OF_COLUMNS, then all atoms  of two layers of the substrate (C , B) and the adatom layer are displayed.
    // the reader should try it least one time to set UPPER_LIMIT_FOR_SUBSTRATE_DISPLAY > NUMBER_OF_COLUMNS to have the optical impression of the crystal layers of the 111 surface


    // Chemical constants

# define R 8.3144
# define PREEXPFACTOR_DESORPTION 0
# define PREEXPFACTOR_DIFFUSION 5e12
# define ELECTRON_CHARGE 1.6019e-19
# define K_BOLTZMANN 1.3807e-23
# define H_BAR 1.05457e-34
# define H_PLANCK 6.62606957e-34
# define Pi 3.1415927

#define STROKE_WIDTH 0.1


# define DIFFUSION_ACTIVATION_ENERGY 0.10

# define TEMPERATURE 0


#define NUMBER_OF_DECADES_IN_TIME 4 // if you choose for example 6 your adsorbate will be followed for example  from 0.1 nano seconds to 0.1 milliseconds .

#define NUMBER_OF_FILES_PER_DECADE_IN_TIME 5 




    // This is the path where your calculated data will be

    // chose "" if your data shall be in the same path  as your excecutable is. This ususally works well for Windows operating systems. Then your data will be in the same folder as your random_walk.exe file. Simply double click on the random_walk.exe file in the Windows Explorer to start the programm.


//		# define PROGRAMM_DIRECTORY "/Users/diesing/programming/output_support_nano_growth_ripen/test/"

	  # define PROGRAMM_DIRECTORY ""






    // the following line is only necessary to display symbol characters in the eps output file
    // in this code it is used to correctly display the greek mu for micro


char introduce_unicode[]="/ReEncoding\n[\n /.notdef \n /.notdef \n /.notdef \n /.notdef \n /.notdef \n /.notdef \n /.notdef \n /.notdef \n /.notdef \n /.notdef \n /.notdef \n /.notdef \n /.notdef \n /.notdef \n /.notdef \n /.notdef \n /.notdef \n /.notdef \n /.notdef \n /.notdef \n /.notdef \n /.notdef \n /.notdef \n /.notdef \n /.notdef \n /.notdef \n /.notdef \n /.notdef \n /.notdef \n /.notdef \n /.notdef \n /.notdef \n /space \n /exclam \n /quotedbl \n /numbersign \n /dollar \n /percent \n /ampersand \n /quotesingle \n /parenleft \n /parenright \n /asterisk \n /plus \n /comma \n /hyphen \n /period \n /slash \n /zero \n /one \n /two \n /three \n /four \n /five \n /six \n /seven \n /eight \n /nine \n /colon \n /semicolon \n /less \n /equal \n /greater \n /question \n /at \n /A \n /B \n /C \n /D \n /E \n /F \n /G \n /H \n /I \n /J \n /K \n /L \n /M \n /N \n /O \n /P \n /Q \n /R \n /S \n /T \n /U \n /V \n /W \n /X \n /Y \n /Z \n /bracketleft \n /backslash \n /bracketright \n /asciicircum \n /underscore \n /grave \n /a \n /b \n /c \n /d \n /e \n /f \n /g \n /h \n /i \n /j \n /k \n /l \n /m \n /n \n /o \n /p \n /q \n /r \n /s \n /t \n /u \n /v \n /w \n /x \n /y \n /z \n /braceleft \n /bar \n /braceright \n /asciitilde \n /.notdef \n /.notdef \n /.notdef \n /quotesinglbase \n /florin \n /quotedblbase \n /ellipsis \n /dagger \n /daggerdbl \n /circumflex \n /perthousand \n /Scaron \n /guilsinglleft \n /OE \n /.notdef \n /.notdef \n /.notdef \n /.notdef \n /quoteleft \n /quoteright \n /quotedblleft \n /quotedblright \n /bullet \n /endash \n /emdash \n /tilde \n /trademark \n /scaron \n /guilsinglright \n /oe \n /dotlessi \n /.notdef \n /Ydieresis \n /.notdef \n /exclamdown \n /cent \n /sterling \n /currency \n /yen \n /brokenbar \n /section \n /dieresis \n /copyright \n /ordfeminine \n /guillemotleft \n /logicalnot \n /guilsinglright \n /registered \n /macron \n /ring \n /plusminus \n /twosuperior \n /threesuperior \n /acute \n /mu \n /paragraph \n /periodcentered \n /cedilla \n /onesuperior \n /ordmasculine \n /guillemotright \n /onequarter \n /onehalf \n /threequarters \n /questiondown \n /Agrave \n /Aacute \n /Acircumflex \n /Atilde \n /Adieresis \n /Aring \n /AE \n /Ccedilla \n /Egrave \n /Eacute \n /Ecircumflex \n /Edieresis \n /Igrave \n /Iacute \n /Icircumflex \n /Idieresis \n /Eth \n /Ntilde \n /Ograve \n /Oacute \n /Ocircumflex \n /Otilde \n /Odieresis \n /multiply \n /Oslash \n /Ugrave \n /Uacute \n /Ucircumflex \n /Udieresis \n /Yacute \n /Thorn \n /germandbls \n /agrave \n /aacute \n /acircumflex \n /atilde \n /adieresis \n /aring \n /ae \n /ccedilla \n /egrave \n /eacute \n /ecircumflex \n /edieresis \n /igrave \n /iacute \n /icircumflex \n /idieresis \n /eth \n /ntilde \n /ograve \n /oacute \n /ocircumflex \n /otilde \n /odieresis \n /divide \n /oslash \n /ugrave \n /uacute \n /ucircumflex \n /udieresis \n /yacute \n /thorn \n /ydieresis\n]\ndef\n\n/reencodefont\n{\nfindfont dup\nlength dict copy\ndup /Encoding ReEncoding put\ndefinefont\n} bind def ";


struct MCS_STRUCT{
    int **number_of_visits_matrix;
    double *logarithmic_time_values;
    int number_mc_step;
    int index_svg_file;
    int i_coordinate_adsorbate; // i coordinate of chosen atom to be moved
    int j_coordinate_adsorbate; // j coordinate of chosen atom to be moved
    double random_number; //random number values chosen by procedure mc_step
    double mc_time;
    double time_increment;
    double temperature;
    double single_adsorbate_diffusion_time;


} mcs;


int j_periodic(int j)
{
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


int i_periodic(int i)
{
    int output=0;
    if (i>=1 && i<=NUMBER_OF_ROWS) {
        output=i;

    }

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


int random_1_6(void)
{
    int output_value;

    output_value=1+(rand() % 6);

    return output_value;
}



void data_output_eps(struct MCS_STRUCT *ptr_to_mcs){

    ptr_to_mcs=&mcs;

    FILE *fhd1;

    int i=0,j=0,actual_i=0,actual_j=0;

    char local_filename[200]="";

    double x_coordinate=0.0,y_coordinate=0.0;

    int font_size_legend=9;

    char fileNamePrefix[200];


        //  a double column figure in a journal typically has a width of 16 cm
        //  this means 16/2.54 inches
        //  Postscript default resolution 72 points pro inch

    double final_page_width=16./2.54*72;;

    double tentative_page_height=final_page_width/(sqrt(2.));


    double relative_sphere_radius=1.0;

    double sphere_radius=tentative_page_height/(2.*relative_sphere_radius+NUMBER_OF_ROWS*sqrt(3)*relative_sphere_radius);


    double measured_page_height=sphere_radius*(2.-sqrt(3)/3.+sqrt(3)*NUMBER_OF_ROWS);

    double measured_page_width=4*sphere_radius+(NUMBER_OF_COLUMNS+NUMBER_OF_ROWS/2.-3./2.)*sphere_radius*2;


    double correctur_factor=measured_page_width/measured_page_height;

    double page_width=final_page_width;

    double page_height=page_width/correctur_factor;


        // recalculation of the sphere radius


    sphere_radius=page_height/(2.*relative_sphere_radius+NUMBER_OF_ROWS*sqrt(3)*relative_sphere_radius);


        // Layer C is shifted towards layer B //

        // The shift in x-direction is called delta_x , its value is sphere_radius
        // The shift in x-direction is called delta_y , its value is minus sphere_radius times sqrt(3)/3
        // the later comes from the geometrical calculation that the layer C atom is just in the middle of
        // an equilateral triangle which is spanned by three atoms of the layer B (the layer below C)

    double  delta_x=sphere_radius;

    double  delta_y=-sphere_radius*tan(M_PI/6);



    double line_thickness_left_hand_legend=font_size_legend/2*0.2;

    double line_thickness_start_actual_position=sphere_radius*0.2;

    double line_spacing=1.5*font_size_legend;

    int start_diffusion_i_index=(int)(ceil(NUMBER_OF_ROWS/2.));

    int start_diffusion_j_index=(int)(ceil(NUMBER_OF_COLUMNS/2.));



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


        //    double lower_left_corner_color_bar_x=page_width-150,lower_left_corner_color_bar_y= -150;


    double y_value_lowest_row=font_size_legend*1;


    double cyan_part_of_cmyk_value=1.0,magenta_part_of_cmyk_value=1.0,yellow_part_of_cmyk_value=1.0,black_part_of_cmyk_value=1.0;


    int maximum_visits=0,minimum_visits=0,number_of_visits_local;






        // find the maximum and the minimum value of localization times

    for (i=1; i<=NUMBER_OF_ROWS; i++) {
        for (j=1; j<=NUMBER_OF_COLUMNS; j++) {

            if (mcs.number_of_visits_matrix[i][j]>=maximum_visits) {

                maximum_visits=mcs.number_of_visits_matrix[i][j];

            }

            else{   minimum_visits=mcs.number_of_visits_matrix[i][j]; }




        }

    }




        // set the filename


    if (mcs.mc_time>1e-12 && mcs.mc_time<1e-9) {



        sprintf(fileNamePrefix,"random_walk_mc_steps_%07d_temp_%3i_K_time_%03i_pico_sec",mcs.number_mc_step,TEMPERATURE,(int)(mcs.mc_time*1e12));



    }


    if (mcs.mc_time>1e-9 && mcs.mc_time<1e-6) {



        sprintf(fileNamePrefix,"random_walk_mc_steps_%07d_temp_%3i_K_time_%03li_nano_sec",mcs.number_mc_step,TEMPERATURE,lround(mcs.mc_time*1e9));



    }


    if (mcs.mc_time>1e-6 && mcs.mc_time<1e-3) {



        sprintf(fileNamePrefix,"random_walk_mc_steps_%07d_temp_%3i_K_time_%03li_mikro_sec",mcs.number_mc_step,TEMPERATURE,lround(mcs.mc_time*1e6));



    }



    if (mcs.mc_time>1e-3 && mcs.mc_time<1) {



        sprintf(fileNamePrefix,"random_walk_mc_steps_%07d_temp_%3i_K_time_%03g_milli_sec",mcs.number_mc_step,TEMPERATURE,round(mcs.mc_time*1e3));



    }



    if (mcs.mc_time>1 ) {



        sprintf(fileNamePrefix,"random_walk_mc_steps_%07d_temp_%3i_K_time_%03i_sec_%03i_milli_sec",mcs.number_mc_step,TEMPERATURE,(int)(trunc(mcs.mc_time)),(int)((mcs.mc_time-trunc(mcs.mc_time))*1000 ));



    }





    strcat(local_filename,PROGRAMM_DIRECTORY);
    strcat(local_filename,"");
        // fprintf(stdout,"%s\n",local_filename);
    strcat(local_filename,fileNamePrefix);

    strcat(local_filename,".eps");





    fhd1=fopen(local_filename,"w+");


    if (fhd1==NULL) {

        printf("Error in opening the file with name\n -%s- ",local_filename);

        printf("\n\nplease check line number 47 in the programm code \n may be add or remove a / ? \n\n ");


    }

    else {


        printf("\n\n A file  named \n -->%s<-- \nwas successfully written \n ",local_filename);


    }










    fprintf(fhd1,"%%!PS-Adobe-2.0 EPSF-2.0\n");
        //fprintf(fhd1,"%%!\n");
    fprintf(fhd1,"%%");fprintf(fhd1,"%%");
    fprintf(fhd1,"BoundingBox: 0 0 %d  %d \n", (int)(ceil(page_width)) ,(int)(ceil(page_height)) );
    fprintf(fhd1,"%%");fprintf(fhd1,"%%");
    fprintf(fhd1,"Creator: University of Duisburg-Essen\n");
    fprintf(fhd1,"%%");fprintf(fhd1,"%%");
    fprintf(fhd1,"For: American Chemical Society\n");
    fprintf(fhd1,"%%");fprintf(fhd1,"%%");
    fprintf(fhd1,"EndComments\n");

    fprintf(fhd1,"%g %g translate \n",0.,page_height);

        // start to write the crystal in the upper left edge of the output page


        // in case t > 1 µ second add the following line to enable unicode mu

    if (mcs.mc_time>1e-6 && mcs.mc_time<1e-3) {

        fprintf(fhd1,"%s\n",introduce_unicode);

    }

      if (NUMBER_OF_COLUMNS<UPPER_LIMIT_FOR_SUBSTRATE_DISPLAY) {

          // the following double for loop writes the substrate layer B

        // this layer is not shifted

    fprintf(fhd1,"0.0 0.0 0.0 0.75 setcmykcolor\n");

    for (i=1; i<=NUMBER_OF_ROWS; i++) {

        for (j=1; j<=NUMBER_OF_COLUMNS; j++){

            x_coordinate=(double)(sphere_radius+((j-1)+(i-1)/2.0)*(sphere_radius*2));
                // the upper line is the shift of each row with index i >1


            y_coordinate=(double)(-1*(sphere_radius+1*((i-1)*sphere_radius*sqrt(3))));


            fprintf(fhd1," %g %g moveto \n",x_coordinate,y_coordinate);
            fprintf(fhd1,"%g %g %g %g %g arc \n",x_coordinate,y_coordinate,sphere_radius,0.,360.);



        };

        fprintf(fhd1, "closepath fill\n");
    }


      }



        // the following double for loop writes the substrate layer C

            // Layer C is shifted versus layer B



        {
        fprintf(fhd1,"0.0 0.0 0.0 0.5 setcmykcolor\n");
            // this is light gray

        for (i=1; i<=NUMBER_OF_ROWS; i++) {

            for (j=1; j<=NUMBER_OF_COLUMNS; j++){

                if (NUMBER_OF_COLUMNS<UPPER_LIMIT_FOR_SUBSTRATE_DISPLAY) {


                    x_coordinate=(double)(delta_x+sphere_radius+((j-1)+(i-1)/2.0)*(sphere_radius*2));

                    y_coordinate=(double) (delta_y-1*(sphere_radius+(i-1)*sphere_radius*sqrt(3)));


                    fprintf(fhd1," %g %g moveto \n",x_coordinate,y_coordinate);

                    fprintf(fhd1,"%g %g %g %i %i arc \n",x_coordinate,y_coordinate,sphere_radius,0,360);

                    fprintf(fhd1,"closepath fill\n");

                }


                else {


                    if (i==1 || i == NUMBER_OF_COLUMNS || j==1 || j== NUMBER_OF_ROWS) {

                        x_coordinate=(double)(delta_x+sphere_radius+((j-1)+(i-1)/2.0)*(sphere_radius*2));

                        y_coordinate=(double) (delta_y-1*(sphere_radius+(i-1)*sphere_radius*sqrt(3)));


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




        // The following double for loops writes the localization_matrix

        // The adsorbate lattice has to be shifted versus substrate layer B
        // So there is 2*delta_x in the x coordinate and 2*delta_y in the y coordinate


        // now  follow the atom on the surface



    for (i=1; i<=NUMBER_OF_ROWS; i++) {

        for (j=1; j<=NUMBER_OF_COLUMNS; j++){

            if(mcs.number_of_visits_matrix[i][j]>0 ){

                x_coordinate=(double) (2*delta_x+sphere_radius+((j-1)+(i-1)/2.0)*(sphere_radius*2));
                    // the upper line is the shift of each row with index i >1


                y_coordinate=(double) (2*delta_y-1*(sphere_radius+1*((i-1)*sphere_radius*2*pow(3,0.5)/2.0)));

                number_of_visits_local=mcs.number_of_visits_matrix[i][j];



                if (number_of_visits_local==1) {
                    cyan_part_of_cmyk_value=0.0;
                    magenta_part_of_cmyk_value=1.0;
                    yellow_part_of_cmyk_value=1.0;
                    black_part_of_cmyk_value=1.0;
                }

                if (number_of_visits_local==2) {
                    cyan_part_of_cmyk_value=0.0;
                    magenta_part_of_cmyk_value=1.0;
                    yellow_part_of_cmyk_value=1.0;
                    black_part_of_cmyk_value=0.75;
                }
                if (number_of_visits_local==3) {
                    cyan_part_of_cmyk_value=0.0;
                    magenta_part_of_cmyk_value=1.0;
                    yellow_part_of_cmyk_value=1.0;
                    black_part_of_cmyk_value=0.5;
                }
                if (number_of_visits_local==4) {
                    cyan_part_of_cmyk_value=0.0;
                    magenta_part_of_cmyk_value=1.0;
                    yellow_part_of_cmyk_value=1.0;
                    black_part_of_cmyk_value=0.25;
                }
                if (number_of_visits_local==5) {
                    cyan_part_of_cmyk_value=0.0;
                    magenta_part_of_cmyk_value=1.0;
                    yellow_part_of_cmyk_value=1.0;
                    black_part_of_cmyk_value=0.0;
                }


                if (number_of_visits_local==6) {
                    cyan_part_of_cmyk_value=0.0;
                    magenta_part_of_cmyk_value=0.75;
                    yellow_part_of_cmyk_value=1.0;
                    black_part_of_cmyk_value=0.0;

                }
                if (number_of_visits_local==7) {
                    cyan_part_of_cmyk_value=0.0;
                    magenta_part_of_cmyk_value=0.5;
                    yellow_part_of_cmyk_value=1.0;
                    black_part_of_cmyk_value=0.0;
                }
                if (number_of_visits_local==8) {
                    cyan_part_of_cmyk_value=0.0;
                    magenta_part_of_cmyk_value=0.25;
                    yellow_part_of_cmyk_value=1.0;
                    black_part_of_cmyk_value=0.0;
                }

                if (number_of_visits_local>8) {
                    cyan_part_of_cmyk_value=0.0;
                    magenta_part_of_cmyk_value=0.0;
                    yellow_part_of_cmyk_value=1.0;
                    black_part_of_cmyk_value=0.0;

                }



                fprintf(fhd1," %g %g %g %g setcmykcolor \n",cyan_part_of_cmyk_value,magenta_part_of_cmyk_value,yellow_part_of_cmyk_value,black_part_of_cmyk_value);

                fprintf(fhd1,"%g %g moveto \n",x_coordinate,y_coordinate);

                fprintf(fhd1,"%g %g %g %i %i arc \n",x_coordinate,y_coordinate,sphere_radius,0,360);

                fprintf(fhd1, "closepath fill\n ");



            }






        }

            // fprintf(stdout,"\n");

    }






        // Finally write the start position symbol in the middle of our adsorbate surfaces. There the single particle starts to diffuse.

    fprintf(fhd1,"0.0 0.0 0.0 1.0 setcmykcolor\n");

    x_coordinate=(double)(2*delta_x+sphere_radius+(( start_diffusion_j_index -1)+( start_diffusion_i_index-1)/2.0)*(sphere_radius*2));
        // the upper line is the shift of each row with index i >1

    y_coordinate=(double)(2*delta_y-1*(sphere_radius+1*(start_diffusion_i_index -1)*sphere_radius*2*pow(3,0.5)/2.0));

    fprintf(fhd1," %g %g moveto \n",x_coordinate,y_coordinate);



    fprintf(fhd1,"%g %g %g %i %i arc \n",x_coordinate,y_coordinate,sphere_radius,0,360);

    fprintf(fhd1, "closepath fill\n ");

    fprintf(fhd1,"0.0 0.0 0.0 0.0 setcmykcolor \n");

    fprintf(fhd1," %g %g moveto \n",x_coordinate,y_coordinate);


    fprintf(fhd1,"%g %g %g %i %i arc \n",x_coordinate,y_coordinate,sphere_radius*0.8,0,360);

    fprintf(fhd1, "closepath fill\n ");


    fprintf(fhd1,"0.0 0.0 0.0 1.0 setcmykcolor\n");

    fprintf(fhd1,"%g %g moveto \n",x_coordinate,y_coordinate+sphere_radius);

    fprintf(fhd1,"%g setlinewidth\n",line_thickness_start_actual_position);

    fprintf(fhd1,"%g %g rlineto \n",0.,-2.*sphere_radius);

    fprintf(fhd1, "closepath stroke \n ");






        // Finally write the symbol of  the current position.


    actual_i=mcs.i_coordinate_adsorbate;
    actual_j=mcs.j_coordinate_adsorbate;


    x_coordinate=(double)(2*delta_x+sphere_radius+((actual_j-1)+(actual_i-1)/2.0)*(sphere_radius*2));
        // the upper line is the shift of each row with index i >1

    y_coordinate=(double) (2*delta_y-1*(sphere_radius+1*((actual_i-1)*sphere_radius*2*pow(3,0.5)/2.0)));

    fprintf(fhd1," %g %g moveto \n",x_coordinate,y_coordinate);


    fprintf(fhd1,"%g %g %g %i %i arc \n",x_coordinate,y_coordinate,sphere_radius,0,360);

    fprintf(fhd1, "closepath fill\n ");

    fprintf(fhd1,"0.0 0.0 0.0 0.0 setcmykcolor\n");

    fprintf(fhd1," %g %g moveto \n",x_coordinate,y_coordinate);


    fprintf(fhd1,"%g %g %g %i %i arc \n",x_coordinate,y_coordinate,sphere_radius*0.8,0,360);

    fprintf(fhd1, "closepath fill\n ");


    fprintf(fhd1,"0.0 0.0 0.0 1.0 setcmykcolor \n");

    fprintf(fhd1,"%g %g moveto \n",x_coordinate-sphere_radius,y_coordinate);

    fprintf(fhd1,"%g %g rlineto \n",2.*sphere_radius,0.);

    fprintf(fhd1, "closepath stroke \n ");

        fprintf(fhd1,"%% initial and final position written \n");







    for (i=0; i<=9; i++) {

        if (i==1) {
            cyan_part_of_cmyk_value=0.0,magenta_part_of_cmyk_value=0.0,yellow_part_of_cmyk_value=1.0,black_part_of_cmyk_value=0.0;

            fprintf(fhd1," %g %g %g %g setcmykcolor \n",cyan_part_of_cmyk_value,magenta_part_of_cmyk_value,yellow_part_of_cmyk_value,black_part_of_cmyk_value);

            fprintf(fhd1,"%g %g moveto \n",page_width-font_size_legend,-(i+0.5)*line_spacing);

            fprintf(fhd1,"%g %g %g %i %i arc \n",page_width-font_size_legend, -(i+0.5)*line_spacing,font_size_legend/2.,0,360);

            fprintf(fhd1, "closepath fill\n ");
        }


        else if (i==2) {
            cyan_part_of_cmyk_value=0.0,magenta_part_of_cmyk_value=0.25,yellow_part_of_cmyk_value=1.0,black_part_of_cmyk_value=0.0;

            fprintf(fhd1," %g %g %g %g setcmykcolor \n",cyan_part_of_cmyk_value,magenta_part_of_cmyk_value,yellow_part_of_cmyk_value,black_part_of_cmyk_value);


            fprintf(fhd1,"%g %g moveto \n",page_width-font_size_legend,-(i+0.5)*line_spacing);

            fprintf(fhd1,"%g %g %g %i %i arc \n",page_width-font_size_legend,-(i+0.5)*line_spacing,font_size_legend/2.,0,360);

            fprintf(fhd1, "closepath fill\n ");


        }


        else if (i==3) {
            cyan_part_of_cmyk_value=0.0,magenta_part_of_cmyk_value=0.5,yellow_part_of_cmyk_value=1.0,black_part_of_cmyk_value=0.0;

            fprintf(fhd1," %g %g %g %g setcmykcolor \n",cyan_part_of_cmyk_value,magenta_part_of_cmyk_value,yellow_part_of_cmyk_value,black_part_of_cmyk_value);

            fprintf(fhd1,"%g %g moveto \n",page_width-font_size_legend,-(i+0.5)*line_spacing);

            fprintf(fhd1,"%g %g %g %i %i arc \n",page_width-font_size_legend,-(i+0.5)*line_spacing,font_size_legend/2.,0,360);

            fprintf(fhd1, "closepath fill\n ");


        }


        else if (i==4) {
            cyan_part_of_cmyk_value=0.0,magenta_part_of_cmyk_value=0.75,yellow_part_of_cmyk_value=1.0,black_part_of_cmyk_value=0.0;

            fprintf(fhd1," %g %g %g %g setcmykcolor \n",cyan_part_of_cmyk_value,magenta_part_of_cmyk_value,yellow_part_of_cmyk_value,black_part_of_cmyk_value);

            fprintf(fhd1,"%g %g moveto \n",page_width-font_size_legend,-(i+0.5)*line_spacing);


            fprintf(fhd1,"%g %g %g %i %i arc \n",page_width-font_size_legend,-(i+0.5)*line_spacing,font_size_legend/2.,0,360);

            fprintf(fhd1, "closepath fill\n ");


        }


        else if (i==5) {
            cyan_part_of_cmyk_value=0.0,magenta_part_of_cmyk_value=1.0,yellow_part_of_cmyk_value=1.0,black_part_of_cmyk_value=0.0;

            fprintf(fhd1," %g %g %g %g setcmykcolor \n",cyan_part_of_cmyk_value,magenta_part_of_cmyk_value,yellow_part_of_cmyk_value,black_part_of_cmyk_value);

            fprintf(fhd1,"%g %g moveto \n",page_width-font_size_legend,-(i+0.5)*line_spacing);


            fprintf(fhd1,"%g %g %g %i %i arc \n",page_width-font_size_legend,-(i+0.5)*line_spacing,font_size_legend/2.,0,360);

            fprintf(fhd1, "closepath fill\n ");

        }

        else if (i==6) {
            cyan_part_of_cmyk_value=0.0,magenta_part_of_cmyk_value=1.0,yellow_part_of_cmyk_value=1.0,black_part_of_cmyk_value=0.25;

            fprintf(fhd1," %g %g %g %g setcmykcolor \n",cyan_part_of_cmyk_value,magenta_part_of_cmyk_value,yellow_part_of_cmyk_value,black_part_of_cmyk_value);

            fprintf(fhd1,"%g %g moveto \n",page_width-font_size_legend,-(i+0.5)*line_spacing);


            fprintf(fhd1,"%g %g %g %i %i arc \n",page_width-font_size_legend,-(i+0.5)*line_spacing,font_size_legend/2.,0,360);

            fprintf(fhd1, "closepath fill\n ");

        }


        else if (i==7) {
            cyan_part_of_cmyk_value=0.0,magenta_part_of_cmyk_value=1.0,yellow_part_of_cmyk_value=1.0,black_part_of_cmyk_value=0.5;

            fprintf(fhd1," %g %g %g %g setcmykcolor \n",cyan_part_of_cmyk_value,magenta_part_of_cmyk_value,yellow_part_of_cmyk_value,black_part_of_cmyk_value);

            fprintf(fhd1,"%g %g moveto \n",page_width-font_size_legend,-(i+0.5)*line_spacing);


            fprintf(fhd1,"%g %g %g %i %i arc \n",page_width-font_size_legend,-(i+0.5)*line_spacing,font_size_legend/2.,0,360);

            fprintf(fhd1, "closepath fill\n ");

        }


        else if (i==8) {
            cyan_part_of_cmyk_value=0.0,magenta_part_of_cmyk_value=1.0,yellow_part_of_cmyk_value=1.0,black_part_of_cmyk_value=0.75;

            fprintf(fhd1," %g %g %g %g setcmykcolor \n",cyan_part_of_cmyk_value,magenta_part_of_cmyk_value,yellow_part_of_cmyk_value,black_part_of_cmyk_value);

            fprintf(fhd1,"%g %g moveto \n",page_width-font_size_legend,-(i+0.5)*line_spacing);


            fprintf(fhd1,"%g %g %g %i %i arc \n",page_width-font_size_legend,-(i+0.5)*line_spacing,font_size_legend/2.,0,360);

            fprintf(fhd1, "closepath fill\n ");

        }

        else if (i>=9) {
            cyan_part_of_cmyk_value=0.0,magenta_part_of_cmyk_value=1.0,yellow_part_of_cmyk_value=1.0,black_part_of_cmyk_value=1.0;

            fprintf(fhd1," %g %g %g %g setcmykcolor \n",cyan_part_of_cmyk_value,magenta_part_of_cmyk_value,yellow_part_of_cmyk_value,black_part_of_cmyk_value);


            fprintf(fhd1,"%g %g moveto \n",page_width-font_size_legend,-(i+0.5)*line_spacing);


            fprintf(fhd1,"%g %g %g %i %i arc \n",page_width-font_size_legend,-(i+0.5)*line_spacing,font_size_legend/2.,0,360);

            fprintf(fhd1, "closepath fill\n ");

        }




    }


        fprintf(fhd1," %%  test \n");







        // the right hand side legend

    fprintf(fhd1,"/Helvetica %d selectfont\n",font_size_legend);

    fprintf(fhd1,"0. 0. 0. 1. setcmykcolor\n");

        // go to left edge of the yellow sphere with more than 8 visits

    fprintf(fhd1,"%g %g moveto \n",page_width-2*font_size_legend, -0.5*line_spacing-font_size_legend/3.);


        // measure how long the printed string will be and print it

    fprintf(fhd1,"(Visits:)stringwidth pop\n  ");

        // move to the left

    fprintf(fhd1,"neg 0 rmoveto   \n");

    fprintf(fhd1," (Visits:) show\n  ");



        // go to left edge of the sphere with 9 visits

    fprintf(fhd1,"%g %g moveto \n",page_width-2*font_size_legend, -1.5*line_spacing-font_size_legend/3.);

        // measure how long the printed string will be and print it

    fprintf(fhd1,"(9 or more)stringwidth pop\n  ");

        // move to the left

    fprintf(fhd1,"neg 0 rmoveto   \n");

    fprintf(fhd1," (9 or more) show\n  ");


        // go to left edge of the sphere with 8 visits

    fprintf(fhd1,"%g %g moveto \n",page_width-2*font_size_legend, -2.5*line_spacing-font_size_legend/3.);

        // measure how long the printed string will be and print it

    fprintf(fhd1,"(8)stringwidth pop\n  ");

        // move to the left

    fprintf(fhd1,"neg 0 rmoveto   \n");

    fprintf(fhd1," (8) show\n  ");



        // go to left edge of the sphere with 7 visits

    fprintf(fhd1,"%g %g moveto \n",page_width-2*font_size_legend, -3.5*line_spacing-font_size_legend/3.);

        // measure how long the printed string will be and print it

    fprintf(fhd1,"(7)stringwidth pop\n  ");

        // move to the left

    fprintf(fhd1,"neg 0 rmoveto   \n");

    fprintf(fhd1," (7) show\n  ");


        // go to left edge of the sphere with 6 visits

    fprintf(fhd1,"%g %g moveto \n",page_width-2*font_size_legend, -4.5*line_spacing-font_size_legend/3.);

        // measure how long the printed string will be and print it

    fprintf(fhd1,"(6)stringwidth pop\n  ");

        // move to the left

    fprintf(fhd1,"neg 0 rmoveto   \n");

    fprintf(fhd1," (6) show\n  ");


        // go to left edge of the sphere with 5 visits

    fprintf(fhd1,"%g %g moveto \n",page_width-2*font_size_legend, -5.5*line_spacing-font_size_legend/3.);

        // measure how long the printed string will be and print it

    fprintf(fhd1,"(5)stringwidth pop\n  ");

        // move to the left

    fprintf(fhd1,"neg 0 rmoveto   \n");

    fprintf(fhd1," (5) show\n  ");


        // go to left edge of the sphere with 4 visits

    fprintf(fhd1,"%g %g moveto \n",page_width-2*font_size_legend, -6.5*line_spacing-font_size_legend/3.);

        // measure how long the printed string will be and print it

    fprintf(fhd1,"(4)stringwidth pop\n  ");

        // move to the left

    fprintf(fhd1,"neg 0 rmoveto   \n");

    fprintf(fhd1," (4) show\n  ");


        // go to left edge of the sphere with 3 visits

    fprintf(fhd1,"%g %g moveto \n",page_width-2*font_size_legend, -7.5*line_spacing-font_size_legend/3.);

        // measure how long the printed string will be and print it

    fprintf(fhd1,"(3)stringwidth pop\n  ");

        // move to the left

    fprintf(fhd1,"neg 0 rmoveto   \n");

    fprintf(fhd1," (3) show\n  ");


        // go to left edge of the sphere with 2 visits

    fprintf(fhd1,"%g %g moveto \n",page_width-2*font_size_legend, -8.5*line_spacing-font_size_legend/3.);

        // measure how long the printed string will be and print it

    fprintf(fhd1,"(2)stringwidth pop\n  ");

        // move to the left

    fprintf(fhd1,"neg 0 rmoveto   \n");

    fprintf(fhd1," (2) show\n  ");



        // go to left edge of the sphere with 1 visits

    fprintf(fhd1,"%g %g moveto \n",page_width-2*font_size_legend, -9.5*line_spacing-font_size_legend/3.);

        // measure how long the printed string will be and print it

    fprintf(fhd1,"(1)stringwidth pop\n  ");

        // move to the left

    fprintf(fhd1,"neg 0 rmoveto   \n");

    fprintf(fhd1," (1) show\n  ");








        fprintf(fhd1, " %g %g translate \n", 0.0,-page_height );





        // write the left hand legend

        // the Layer B  at 5 * line spacing  and Layer C at 4 * line spacing



        if (NUMBER_OF_COLUMNS< UPPER_LIMIT_FOR_SUBSTRATE_DISPLAY) {

        fprintf(fhd1," 0.0 0.0 0.0 0.75  setcmykcolor\n");

        fprintf(fhd1,"%g %g moveto \n",font_size_legend+1.5,y_value_lowest_row+5*line_spacing);

        fprintf(fhd1,"%g %g %g 0 360 arc\n closepath fill\n ",font_size_legend+1.5,y_value_lowest_row+5*line_spacing,font_size_legend/2.);

        fprintf(fhd1," 0. 0. 0. 1.0 setcmykcolor\n");

        fprintf(fhd1,"%g %g moveto \n (Layer B) show\n  \n",font_size_legend*2.0,y_value_lowest_row+5*line_spacing-font_size_legend/2.5);

        }



        fprintf(fhd1," 0.0 0.0 0.0 0.5 setcmykcolor\n");

        fprintf(fhd1,"%g %g moveto \n",font_size_legend+1.5,y_value_lowest_row+4*line_spacing);

        fprintf(fhd1,"%g %g %g 0 360 arc\nclosepath fill\n ",font_size_legend+1.5,y_value_lowest_row+4*line_spacing,font_size_legend/2.);

        fprintf(fhd1," 0. 0. 0. 1.0 setcmykcolor\n");

        fprintf(fhd1,"%g %g moveto \n (Layer C) show\n",font_size_legend*2.0,y_value_lowest_row+4*line_spacing-font_size_legend/2.5);

    }





        // Write the start position symbol in the legend at y = 3 * line spacing


    fprintf(fhd1,"%g %g moveto \n",font_size_legend+1.5,y_value_lowest_row+3*line_spacing);

    fprintf(fhd1,"%g %g %g 0 360 arc\nclosepath fill\n ",font_size_legend+1.5,y_value_lowest_row+3*line_spacing,font_size_legend/2*1.0);

    fprintf(fhd1," 0.0 0.0 0.0 0.0 setcmykcolor\n");

    fprintf(fhd1,"%g %g moveto \n",font_size_legend+1.5,y_value_lowest_row+3*line_spacing);

    fprintf(fhd1,"%g %g %g 0 360 arc\n closepath fill\n ",font_size_legend+1.5,y_value_lowest_row+3*line_spacing,font_size_legend/2*0.8);

    fprintf(fhd1," 0.0 0.0 0.0 1.0 setcmykcolor\n");

    fprintf(fhd1," %g %g  moveto ",font_size_legend+1.5,y_value_lowest_row+3*line_spacing-font_size_legend/2);

    fprintf(fhd1," %g setlinewidth\n ",line_thickness_left_hand_legend);

    fprintf(fhd1," %g %g rlineto \n closepath stroke \n",0.,(double)(font_size_legend*1-1));

    fprintf(fhd1,"%g %g moveto \n (Start position) show\n",font_size_legend*2.0,y_value_lowest_row+3*line_spacing-font_size_legend/2.5);


        // Write the final position symbol in the legend at 2 * line spacing


    fprintf(fhd1," %g %g  moveto ",font_size_legend+1.5,y_value_lowest_row+2*line_spacing);

    fprintf(fhd1,"%g %g %g 0 360 arc\n closepath fill\n ",font_size_legend+1.5,y_value_lowest_row+2*line_spacing,font_size_legend/2*1.0);

    fprintf(fhd1," 0.0 0.0 0.0 0.0 setcmykcolor\n");

    fprintf(fhd1," %g %g  moveto ",font_size_legend+1.5,y_value_lowest_row+2*line_spacing);

    fprintf(fhd1,"%g %g %g 0 360 arc\n closepath fill\n ",font_size_legend+1.5,y_value_lowest_row+2*line_spacing,font_size_legend/2*0.8);

    fprintf(fhd1," %g %g moveto ",2.5+font_size_legend*0.5,y_value_lowest_row+2*line_spacing);

    fprintf(fhd1," 0.0 0.0 0.0 1.0 setcmykcolor\n");

    fprintf(fhd1," %g %g rlineto \n closepath stroke \n",(double)(font_size_legend)*1.0-2.0,0.);

    fprintf(fhd1,"%g %g moveto \n (Current position) show\n",font_size_legend*2.0,y_value_lowest_row+2*line_spacing-font_size_legend/2.5);


        // write the totally elapsed time and the temperature during diffusion at 1 * line spacing

    fprintf(fhd1,"%g %g moveto \n",font_size_legend/2.,y_value_lowest_row+line_spacing-font_size_legend/2.5);

    if (mcs.mc_time<1e-9) {
        fprintf(fhd1,"(Time: %3.3g ps )show \n",(double) (mcs.mc_time*1e12));
    }

    if (mcs.mc_time>=1e-9 && mcs.mc_time<1e-6) {
        fprintf(fhd1,"(Time: %3.3g ns )show \n",(double) (mcs.mc_time*1e9));

    }


    if (mcs.mc_time>=1e-6 && mcs.mc_time<1e-3) {


        fprintf(fhd1,"/_Helvetica /Helvetica reencodefont %d scalefont setfont (Time: %3.3g)show\n",font_size_legend,(double) (mcs.mc_time*1e6));

            // fprintf(fhd1,"%g 0 rmoveto\n <00b5>show (s) show \n",-0.4*font_size_legend);
        fprintf(fhd1,"%g 0 rmoveto\n <00b5>show (s) show \n",-0.1*font_size_legend);



    }


    if (mcs.mc_time>=1e-3 && mcs.mc_time<1) {
        fprintf(fhd1,"(Time: %3.3g ms )show \n",(double) (mcs.mc_time*1e3));

    }


    if (mcs.mc_time>=1 && mcs.mc_time<100) {
        fprintf(fhd1,"(Time: %2ds  %3dms )show \n",(int) (mcs.mc_time),(int)((mcs.mc_time - trunc(mcs.mc_time))*1000) );

    }






    fprintf(fhd1,"%g %g moveto (Temperature: %d K)  show \n ",font_size_legend/2.,y_value_lowest_row-font_size_legend/2.5,(int)(TEMPERATURE) );

        //  fprintf(fhd1,"%g %g moveto (Number of MC steps: %d)  show \n ",1.5+font_size_legend/2.,160.,mcs.number_mc_step);

    fclose(fhd1);





}



void data_output_svg(struct MCS_STRUCT *ptr_to_mcs){

	ptr_to_mcs=&mcs;

    mcs.index_svg_file = mcs.index_svg_file + 1;

	FILE *svgFile;
    FILE *csvFile;

    char csvFilename[]="times.csv";

    csvFile = fopen(csvFilename,"a");

    fprintf(csvFile,"%d, %d, %g,%g \n",mcs.index_svg_file,mcs.number_mc_step,mcs.temperature,mcs.mc_time);

    fclose(csvFile);

	int i=0,j=0,actual_i=0,actual_j=0;

   //  char csv_filename[20]="";

	char local_filename[200]="";

	double x_coordinate=0.0,y_coordinate=0.0;

	char fileNamePrefix[200];

	int text_font_size_pts=4;

	double text_font_size_mm=text_font_size_pts*25.4/72.0;

	double text_x_offset=2.0;

	double legend_y_offset=6.0;

	double text_line_spacing=1.5*text_font_size_pts;

		//  a double column figure in a journal typically has a width of 16 cm
		//  this means 16/2.54 inches
		//  Postscript default resolution 72 points pro inch


	//double page_margin=0.1;

//	double stroke_width_page_margin=0.3;

	double final_page_width=82.0*3.0;

	double tentative_page_height=final_page_width/(sqrt(2.));

	double relative_sphere_radius=1.0;

	double sphere_radius=tentative_page_height/(2.*relative_sphere_radius+NUMBER_OF_ROWS*sqrt(3)*relative_sphere_radius);

	double measured_page_height=sphere_radius*(2.-sqrt(3)/3.+sqrt(3)*NUMBER_OF_ROWS);

	double measured_page_width=4*sphere_radius+(NUMBER_OF_COLUMNS+NUMBER_OF_ROWS/2.-3./2.)*sphere_radius*2;

	double correctur_factor=measured_page_width/measured_page_height;

	double page_width=final_page_width;

	double page_height=page_width/correctur_factor;


		// the following two x _coordinates (smallest and largest) are used to make a rectangular output instead of a paralleggramm output

        //	double smallest_x_coordinate=0.0; //=(double)(sphere_radius+((1-1)+(NUMBER_OF_ROWS-1)/2.0)*(sphere_radius*2));

        //	double largest_x_coordinate=0.0; //(double)(sphere_radius+((NUMBER_OF_COLUMNS-1)+(1-1)/2.0)*(sphere_radius*2));;

		// recalculation of the sphere radius


	sphere_radius=page_height/(2.*relative_sphere_radius+NUMBER_OF_ROWS*sqrt(3)*relative_sphere_radius);


		// Layer C is shifted towards layer B //

		// The shift in x-direction is called delta_x , its value is sphere_radius
		// The shift in x-direction is called delta_y , its value is minus sphere_radius times sqrt(3)/3
		// the later comes from the geometrical calculation that the layer C atom is just in the middle of
		// an equilateral triangle which is spanned by three atoms of the layer B (the layer below C)

	double  delta_x=sphere_radius;

	double  delta_y=sphere_radius*tan(M_PI/6);


	int start_diffusion_i_index=(int)(ceil(NUMBER_OF_ROWS/2.));

	int start_diffusion_j_index=(int)(ceil(NUMBER_OF_COLUMNS/2.));

		// define the parallelogramm which replaces layer c for greater matrices

		// seen from the top left corner of the page


	double

	top_left_corner_parallelogram_x=2*sphere_radius,

	top_left_corner_parallelogram_y=sphere_radius*(1.+sqrt(3)/3.),

	bottom_left_corner_parallelogram_x=sphere_radius+sphere_radius*NUMBER_OF_ROWS,

	bottom_left_corner_parallelogram_y=(.1547005390*sphere_radius-1.732050808*sphere_radius*NUMBER_OF_ROWS),

	top_right_corner_parallelogram_x=2*sphere_radius*NUMBER_OF_COLUMNS,

	top_right_corner_parallelogram_y= top_left_corner_parallelogram_y, // ist ungeaendert

	bottom_right_corner_parallelogram_x=-1.*sphere_radius+2.*sphere_radius*NUMBER_OF_COLUMNS+sphere_radius*NUMBER_OF_ROWS,

	bottom_right_corner_parallelogram_y=(.1547005390*sphere_radius+1.732050808*sphere_radius*NUMBER_OF_ROWS);

	double right_legend_x_coor=page_width-55;

		// The layers B, C and the adsorbate layer are shifted vs each other
		// by sphere_radius in x and by sphere_radius*tan(30 deg) in y


	int red_value=0,green_value=0,blue_value=0;

	int maximum_visits=0,minimum_visits=0,number_of_visits_local=0;

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

		//	sprintf(svg_header_line_three,"<svg  viewBox=\"%g %g %g %g\" width=\"%gmm\" height=\"%gmm\" version=\"1.1\" ",(X_OFFSET_PERCENT/100.*imag_par.page_width),(Y_OFFSET_PERCENT/100.*imag_par.page_height) ,(imag_par.page_width/MAGNIFICATION_IN_VIEW_BOX),(imag_par.page_height/MAGNIFICATION_IN_VIEW_BOX),imag_par.page_width,imag_par.page_height);


	sprintf(svg_header_line_three,"<svg  viewBox=\"0 0 %i %i\" width=\"%gmm\" height=\"%gmm\" version=\"1.1\" ",(int)(ceil(page_width)),(int)(ceil(page_height)),page_width,page_height);

   char CSV_filename[]="times.csv";

  


		// find the maximum and the minimum value of localization times

	for (i=1; i<=NUMBER_OF_ROWS; i++) {

		for (j=1; j<=NUMBER_OF_COLUMNS; j++) {

			if (mcs.number_of_visits_matrix[i][j]>=maximum_visits) {

				maximum_visits=mcs.number_of_visits_matrix[i][j];

			}

			else{minimum_visits=mcs.number_of_visits_matrix[i][j]; }

		}

	}




		// set the filename

if (mcs.index_svg_file<10)
{
    sprintf(fileNamePrefix,"m_0%i",mcs.index_svg_file);
}

if (mcs.index_svg_file>10)
{
    sprintf(fileNamePrefix,"m_%i",mcs.index_svg_file);
}



/*

 char *dataDirectoryName[]="";



sprintf(dataDirectoryName,"columns_%i_rows_%i_temperature_%i",NUMBER_OF_COLUMNS,NUMBER_OF_ROWS,TEMPERATURE);

if (mkdir(dataDirectoryName)== -1){
if(mkdir(dataDirectoryName,0700)==-1 )
{
    perror("Error creating directory");
    //return 1;
}
    printf("Directory created successfully");
}

*/


	//strcat(local_filename,dataDirectoryName);
	//strcat(local_filename,"");
		// fprintf(stdout,"%s\n",local_filename);
	strcat(local_filename,fileNamePrefix);

	strcat(local_filename,".svg");





	svgFile=fopen(local_filename,"w+");


	if (svgFile==NULL) {

		printf("Error in opening the file with name\n -%s- ",local_filename);

		printf("\n\nplease check line number 47 in the programm code \n may be add or remove a / ? \n\n ");


	}

	else {


		printf("\n\n A  svg file  named \n -->%s<-- \nwas successfully written \n ",local_filename);


	}



	svgFile=fopen(local_filename,"w+");

	fprintf(svgFile,"%s\n",svg_header_line_one);

	fprintf(svgFile,"%s\n",svg_header_line_two);

	fprintf(svgFile,"%s\n",svg_header_line_three);

	fprintf(svgFile,"%s\n",svg_header_line_four);

	fprintf(svgFile,"%s\n",svg_header_line_five);


		//	fprintf(fhd1," <line x1=\"%g\" y1=\"%g\" x2=\"%g\" y2=\"%g\"  style=\"stroke:red; stroke-width:0.3; stroke-opacity:1.0\" /> \n",page_margin,page_margin, page_width-page_margin,page_height-page_margin);

		//	fprintf(fhd1, "<rect x=\"%g\" y=\"%g\" width=\"%i\" height=\"%i\" style=\"fill:none; stroke:black; stroke-width:0.3;stroke-opacity:1.0\" /> \n",page_margin,page_margin,(int)(floor(page_width-1*stroke_width_page_margin)),(int)(page_height-1*stroke_width_page_margin));




//	fprintf(fhd1, "<rect >  ");


	fprintf(svgFile,"<rect x=\"0\" y=\"0\" fill=\"white\" stroke=\"black\" stroke-width=\"0.2\" width=\"%i\" height=\"%i\"/> ",(int)(ceil(page_width)), (int)(ceil(page_height)));



		// write the substrate layer B with circles everywhere

	if(NUMBER_OF_COLUMNS <= UPPER_LIMIT_FOR_SUBSTRATE_DISPLAY) {

			// write the substrate layer B with circles everywhere

		fprintf(svgFile, "\n<defs>\n");

		fprintf(svgFile, "<g id=\"substrate layer B with circles everywhere\">\n");


		fprintf(svgFile, "\n<defs>\n");

		fprintf(svgFile, "<g id=\"B layer circle\"  style =\"stroke:rgb(64,64,64); stroke-width:%g; fill:rgb(64,64,64);stroke-opacity:1.0;fill-opacity:1.0;\" > \" \n",STROKE_WIDTH);

		fprintf(svgFile,"<circle cx=\"%g\" cy=\"%g\" r=\"%g\" />\n ",0.0,0.0,sphere_radius);

		fprintf(svgFile,"</g>\n\n");

		fprintf(svgFile, "</defs>\n");


		for (i=1; i<=NUMBER_OF_ROWS; i++) {

			for (j=1; j<=NUMBER_OF_COLUMNS; j++){

				x_coordinate=(double)(sphere_radius+((j-1)+(i-1)/2.0)*(sphere_radius*2));
					// the upper line is the shift of each row with index i >1


				y_coordinate=(double)((sphere_radius+1*((i-1)*sphere_radius*sqrt(3))));


				fprintf(svgFile, "<use xlink:href=\"#B layer circle\" x=\"%g\" y=\"%g\"  />\n",x_coordinate,y_coordinate);



			}

		}

		fprintf(svgFile,"</g>\n</defs>\n");

		fprintf(svgFile, "<use xlink:href=\"#substrate layer B with circles everywhere\" transform=\"translate(%g,%g)\" />\n\n",sphere_radius,0.0);

	}




		// write the substrate layer B with outside circles and a parallelogramm inside


	if(NUMBER_OF_COLUMNS > UPPER_LIMIT_FOR_SUBSTRATE_DISPLAY) {



			// write the substrate layer B

		fprintf(svgFile, "\n<defs> \n");

		fprintf(svgFile, "<g id=\"substrate layer B with outside circles\" >\n\n");

		fprintf(svgFile, "\n\n\n\n<defs>\n");

		fprintf(svgFile, "<g id=\"B layer circle\"  style =\"stroke:rgb(64,64,64); stroke-width:%g; fill:rgb(64,64,64);stroke-opacity:1.0;fill-opacity:1.0;\" > \" \n",STROKE_WIDTH);

		fprintf(svgFile,"<circle cx=\"%g\" cy=\"%g\" r=\"%g\" />\n ",0.0,0.0,sphere_radius);

		fprintf(svgFile,"</g>\n\n");

		fprintf(svgFile, "</defs>");



		for (i=1; i<=NUMBER_OF_ROWS; i++) {

			for (j=1; j<=NUMBER_OF_COLUMNS; j++){

				x_coordinate=(double)(sphere_radius+((j-1)+(i-1)/2.0)*(sphere_radius*2));
					// the upper line is the shift of each row with index i >1


				y_coordinate=(double)((sphere_radius+1*((i-1)*sphere_radius*sqrt(3))));

				if (i==1 || i == NUMBER_OF_ROWS || j==1 || j == NUMBER_OF_COLUMNS) {


					fprintf(svgFile, "<use xlink:href=\"#B layer circle\" x=\"%g\" y=\"%g\"  />\n",x_coordinate,y_coordinate);

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


		fprintf(svgFile, "<!-- the parallelogramm for layer B -->\n\n");

		fprintf(svgFile, "<polyline points= \"%g %g, %g %g,%g %g,%g %g \" style = \"stroke:rgb(64,64,64); stroke-width:%g; fill:rgb(64,64,64);stroke-opacity:1.0;fill-opacity:1.0;\"  /> ",top_left_corner_parallelogram_x,top_left_corner_parallelogram_y,bottom_left_corner_parallelogram_x,bottom_left_corner_parallelogram_y,bottom_right_corner_parallelogram_x,bottom_right_corner_parallelogram_y,top_right_corner_parallelogram_x,top_right_corner_parallelogram_y,STROKE_WIDTH);






		fprintf(svgFile,"</g>\n</defs>\n");

		fprintf(svgFile, "<use xlink:href=\"#substrate layer B with outside circles\" transform=\"translate(%g,%g)\" />\n\n",sphere_radius,0.0);







	}





		// write the substrate layer C with circles everywhere

	if (NUMBER_OF_COLUMNS <= UPPER_LIMIT_FOR_SUBSTRATE_DISPLAY) {

			// write the substrate layer C with circles everywhere

		fprintf(svgFile, "\n<defs>\n");

		fprintf(svgFile,"<g id=\"substrate layer C with circles everywhere\">\n");






		fprintf(svgFile, "\n<defs>\n");

		fprintf(svgFile, "<g id=\"C layer circle\"  style =\"stroke:rgb(192,192,192); stroke-width:%g; fill:rgb(192,192,192);stroke-opacity:0.7;fill-opacity:0.7;\" > \" \n",STROKE_WIDTH);

		fprintf(svgFile,"<circle cx=\"%g\" cy=\"%g\" r=\"%g\" />\n ",0.0,0.0,sphere_radius);

		fprintf(svgFile,"</g>\n");

		fprintf(svgFile, "</defs>\n\n\n");



		for (i=1; i<=NUMBER_OF_ROWS; i++) {

			for (j=1; j<=NUMBER_OF_COLUMNS; j++){

				x_coordinate=(double)(delta_x+sphere_radius+((j-1)+(i-1)/2.0)*(sphere_radius*2));

				y_coordinate=(double) (delta_y+1*(sphere_radius+(i-1)*sphere_radius*sqrt(3)));

				fprintf(svgFile, "<use xlink:href=\"#C layer circle\" x=\"%g\" y=\"%g\"  />\n",x_coordinate,y_coordinate);

			}

		}

			//closing the group substrate layer C
		fprintf(svgFile,"</g>\n");
			// closing the the defintion of substrate layer C
		fprintf(svgFile,"\n</defs>\n");


		fprintf(svgFile, "<use xlink:href=\"#substrate layer C with circles everywhere\" transform=\"translate(%g,%g)\" />\n\n",sphere_radius,0.0);


	}



		// write the substrate layer C with outside circles and a parallelogramm inside

	if(NUMBER_OF_COLUMNS > UPPER_LIMIT_FOR_SUBSTRATE_DISPLAY){

			// write the substrate layer C with outside circles and a parallelogram inside


		fprintf(svgFile, "\n<defs>\n");

		fprintf(svgFile,"<g id=\"substrate layer C with outside circles\">\n");


		fprintf(svgFile, "\n<defs>\n");

		fprintf(svgFile, "<g id=\"C layer circle\"  style =\"stroke:rgb(192,192,192); stroke-width:%g; fill:rgb(192,192,192);stroke-opacity:0.7;fill-opacity:0.7;\" > \" \n",STROKE_WIDTH);

		fprintf(svgFile,"<circle cx=\"%g\" cy=\"%g\" r=\"%g\" />\n ",0.0,0.0,sphere_radius);

		fprintf(svgFile,"</g>\n");

		fprintf(svgFile, "</defs>\n\n\n");








		for (i=1; i<=NUMBER_OF_ROWS; i++) {

			for (j=1; j<=NUMBER_OF_COLUMNS; j++){


				x_coordinate=(double)(delta_x+sphere_radius+((j-1)+(i-1)/2.0)*(sphere_radius*2));

				y_coordinate=(double) (delta_y+1*(sphere_radius+(i-1)*sphere_radius*sqrt(3)));


				if (i==1 || i == NUMBER_OF_ROWS || j==1 || j == NUMBER_OF_COLUMNS) {



					fprintf(svgFile, "<use xlink:href=\"#C layer circle\" x=\"%g\" y=\"%g\"  />\n",x_coordinate,y_coordinate);



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



		fprintf(svgFile, "<!-- the parallelogramm for layer C -->\n\n");

		fprintf(svgFile, "<polyline points= \"%g %g, %g %g,%g %g,%g %g \" style = \"stroke:rgb(192,192,192); stroke-width:%g; fill:rgb(192,192,192);stroke-opacity:0.7;fill-opacity:0.7;\"  /> ",top_left_corner_parallelogram_x,top_left_corner_parallelogram_y,bottom_left_corner_parallelogram_x,bottom_left_corner_parallelogram_y,bottom_right_corner_parallelogram_x,bottom_right_corner_parallelogram_y,top_right_corner_parallelogram_x,top_right_corner_parallelogram_y,STROKE_WIDTH);




			//closing the group substrate layer C
		fprintf(svgFile,"</g>\n");
			// closing the the defintion of substrate layer C
		fprintf(svgFile,"\n</defs>\n");


		fprintf(svgFile, "<use xlink:href=\"#substrate layer C with outside circles\" transform=\"translate(%g,%g)\" />\n\n",+sphere_radius,0.0);




	}


















		// The following double for loops writes the localization_matrix

		// The adsorbate lattice has to be shifted versus substrate layer B
		// So there is 2*delta_x in the x coordinate and 2*delta_y in the y coordinate


		// now  follow the atom on the surface


	fprintf(svgFile, "<defs> <g id=\"follow the atom\">\n");


	for (i=1; i<=NUMBER_OF_ROWS; i++) {

		for (j=1; j<=NUMBER_OF_COLUMNS; j++){

			if(mcs.number_of_visits_matrix[i][j]>0 ){

				x_coordinate=(double) (2*delta_x+sphere_radius+((j-1)+(i-1)/2.0)*(sphere_radius*2));
					// the upper line is the shift of each row with index i >1


				y_coordinate=(double) (2*delta_y+1*(sphere_radius+1*((i-1)*sphere_radius*2*pow(3,0.5)/2.0)));

				number_of_visits_local=mcs.number_of_visits_matrix[i][j];


				if (number_of_visits_local==1) {
					red_value=0;
					green_value=0;
					blue_value=0;

				}

				if (number_of_visits_local==2) {
					red_value=165;
					green_value=42;
					blue_value=42;

				}

				if (number_of_visits_local==3) {
					red_value=184;
					green_value=34;
					blue_value=11;

				}


				if (number_of_visits_local==4) {
					red_value=255;
					green_value=64;
					blue_value=0;

				}

				if (number_of_visits_local==5) {
					red_value=255;
					green_value=0;
					blue_value=0;

				}


				if (number_of_visits_local==5) {
					red_value=255;
					green_value=0;
					blue_value=64;

				}

				if (number_of_visits_local==6) {
					red_value=255;
					green_value=64;
					blue_value=0;

				}

				if (number_of_visits_local==7) {
					red_value=255;
					green_value=128;
					blue_value=0;

				}

				if (number_of_visits_local==8) {
					red_value=255;
					green_value=192;
					blue_value=0;

				}


				if (number_of_visits_local==9) {
					red_value=255;
					green_value=255;
					blue_value=0;

				}

				fprintf(svgFile,"<circle cx=\"%g\" cy=\"%g\" r=\"%g\" style=\" stroke:rgb(%i,%i,%i); stroke-width:%g; fill:rgb(%i,%i,%i);stroke-opacity:1.0;fill-opacity:1.0; \"  />\n",x_coordinate,y_coordinate,sphere_radius,red_value,green_value,blue_value,STROKE_WIDTH,red_value,green_value,blue_value);


				fprintf(svgFile,"<!-- the i j coordinates of the line above are [%i,%i] -->\n>",i,j);




			}

		}

	}

	fprintf(svgFile,"</g>\n</defs>\n");

	fprintf(svgFile, "<use xlink:href=\"#follow the atom\" transform=\"translate(%g,%g)\" />\n\n\n",sphere_radius,0.0);





		// Finally write the symbol of  the current position.



	fprintf(svgFile, "<defs> <g id=\"write the actual position\">\n");

	actual_i=mcs.i_coordinate_adsorbate;
	actual_j=mcs.j_coordinate_adsorbate;



	x_coordinate=(double)(2*delta_x+sphere_radius+((actual_j-1)+(actual_i-1)/2.0)*(sphere_radius*2));
		// the upper line is the shift of each row with index i >1

	y_coordinate=(double) (2*delta_y+1*(sphere_radius+1*((actual_i-1)*sphere_radius*2*pow(3,0.5)/2.0)));


	fprintf(svgFile,"<circle cx=\"%g\" cy=\"%g\" r=\"%g\" style=\" stroke:rgb(0,0,0); stroke-width:%g; fill:rgb(255,255,255);stroke-opacity:1.0;fill-opacity:1.0; \"  />\n",x_coordinate,y_coordinate,sphere_radius,STROKE_WIDTH);

	fprintf(svgFile, "<!--The atom i j coordinates of the line above are [%i,%i] -->\n",actual_i,actual_j);


	fprintf(svgFile, "<line x1=\"%g\" y1=\"%g\" x2=\"%g\" y2=\"%g\"  style=\"stroke:black; stroke-width:%g; stroke-opacity:1.0\" /> \n",x_coordinate,y_coordinate-sphere_radius,x_coordinate,y_coordinate+sphere_radius,STROKE_WIDTH);


	fprintf(svgFile,"</g>\n</defs>\n");

	fprintf(svgFile, "<use xlink:href=\"#write the actual position\" transform=\"translate(%g,%g)\" />\n\n",+sphere_radius,0.0);










		// Finally write the start position symbol in the middle of our adsorbate surfaces. There the single particle starts to diffuse.


	fprintf(svgFile, "<defs> <g id=\"write the start position\">\n");



	x_coordinate=(double)(2*delta_x+sphere_radius+(( start_diffusion_j_index -1)+( start_diffusion_i_index-1)/2.0)*(sphere_radius*2));
		// the upper line is the shift of each row with index i >1

	y_coordinate=(double)(2*delta_y+1*(sphere_radius+1*(start_diffusion_i_index -1)*sphere_radius*2*pow(3,0.5)/2.0));

	fprintf(svgFile,"<circle cx=\"%g\" cy=\"%g\" r=\"%g\" style=\" stroke:rgb(0,0,0); stroke-width:%g; fill:rgb(255,255,255);stroke-opacity:1.0;fill-opacity:1.0; \"  />\n",x_coordinate,y_coordinate,sphere_radius,STROKE_WIDTH);


	fprintf(svgFile, "<!--The atom i j coordinates of the line above are [%i,%i] -->\n",start_diffusion_i_index,start_diffusion_j_index);


	fprintf(svgFile, "<line x1=\"%g\" y1=\"%g\" x2=\"%g\" y2=\"%g\"  style=\"stroke:black; stroke-width:%g; stroke-opacity:1.0\" /> \n",x_coordinate-sphere_radius,y_coordinate,x_coordinate+sphere_radius,y_coordinate,STROKE_WIDTH);



	fprintf(svgFile,"</g>\n</defs>\n");

	fprintf(svgFile, "<use xlink:href=\"#write the start position\" transform=\"translate(%g,%g)\" />",sphere_radius,0.0);














	fprintf(svgFile, "<defs> <g id=\"write the legend\" style=\"font-size:%ipt;font-family:Helvetica\" >\n",text_font_size_pts);


	fprintf(svgFile, "<text x=\"%g\"  y=\"%g\" >\n ",right_legend_x_coor,legend_y_offset);

	fprintf(svgFile, " Zahl der Aufenthalte: \n");

	fprintf(svgFile, "</text>");






	fprintf(svgFile, "<circle cx=\"%g\" cy=\"%g\" r=\"%g\"  style=\"stroke:rgb(%i,%i,%i);fill:rgb(%i,%i,%i); \"  />",page_width-11.0,legend_y_offset+text_line_spacing,1.0,0,0,0,0,0,0);

	fprintf(svgFile, "<text x=\"%g\"  y=\"%g\" >\n ",page_width-9.0,legend_y_offset+text_line_spacing+text_font_size_mm);

	fprintf(svgFile, " 1 \n");

	fprintf(svgFile, "</text>");


	fprintf(svgFile, "<circle cx=\"%g\" cy=\"%g\" r=\"%g\"  style=\"stroke:rgb(%i,%i,%i);fill:rgb(%i,%i,%i); \"  />",page_width-11.0,legend_y_offset+2.0*text_line_spacing,1.0,145,42,42,145,42,42);

	fprintf(svgFile, "<text x=\"%g\"  y=\"%g\" >\n ",page_width-9.0,legend_y_offset+2.0*text_line_spacing+text_font_size_mm);

	fprintf(svgFile, " 2 \n");

	fprintf(svgFile, "</text>");



	fprintf(svgFile, "<circle cx=\"%g\" cy=\"%g\" r=\"%g\"  style=\"stroke:rgb(%i,%i,%i);fill:rgb(%i,%i,%i); \"  />",page_width-11.0,legend_y_offset+3.0*text_line_spacing,1.0,194,34,11,194,34,11);

	fprintf(svgFile, "<text x=\"%g\"  y=\"%g\" >\n ",page_width-9.0,legend_y_offset+3.0*text_line_spacing+text_font_size_mm);

	fprintf(svgFile, " 3 \n");

	fprintf(svgFile, "</text>");



	fprintf(svgFile, "<circle cx=\"%g\" cy=\"%g\" r=\"%g\"  style=\"stroke:rgb(%i,%i,%i);fill:rgb(%i,%i,%i); \"  />",page_width-11.0,legend_y_offset+4.0*text_line_spacing,1.0,2444,34,11,2444,34,11);

	fprintf(svgFile, "<text x=\"%g\"  y=\"%g\" >\n ",page_width-9.0,legend_y_offset+4.0*text_line_spacing+text_font_size_mm);

	fprintf(svgFile, " 4 \n");

	fprintf(svgFile, "</text>");




	fprintf(svgFile, "<circle cx=\"%g\" cy=\"%g\" r=\"%g\"  style=\"stroke:rgb(%i,%i,%i);fill:rgb(%i,%i,%i); \"  />",page_width-11.0,legend_y_offset+5.0*text_line_spacing,1.0,255,44,84,255,44,84);

	fprintf(svgFile, "<text x=\"%g\"  y=\"%g\" >\n ",page_width-9.0,legend_y_offset+5.0*text_line_spacing+text_font_size_mm);

	fprintf(svgFile, " 5 \n");

	fprintf(svgFile, "</text>");



	fprintf(svgFile, "<circle cx=\"%g\" cy=\"%g\" r=\"%g\"  style=\"stroke:rgb(%i,%i,%i);fill:rgb(%i,%i,%i); \"  />",page_width-11.0+sphere_radius,legend_y_offset+6.0*text_line_spacing,1.0,255,114,0,255,114,0);

	fprintf(svgFile, "<text x=\"%g\"  y=\"%g\" >\n ",page_width-9.0,legend_y_offset+6.0*text_line_spacing+text_font_size_mm);

	fprintf(svgFile, " 6 \n");

	fprintf(svgFile, "</text>");


	fprintf(svgFile, "<circle cx=\"%g\" cy=\"%g\" r=\"%g\"  style=\"stroke:rgb(%i,%i,%i);fill:rgb(%i,%i,%i); \"  />",page_width-11.0+sphere_radius,legend_y_offset+7.0*text_line_spacing,1.0,255,165,0,255,165,0);

	fprintf(svgFile, "<text x=\"%g\"  y=\"%g\" >\n ",page_width-9.0,legend_y_offset+7.0*text_line_spacing+text_font_size_mm);

	fprintf(svgFile, " 7 \n");

	fprintf(svgFile, "</text>");




	fprintf(svgFile, "<circle cx=\"%g\" cy=\"%g\" r=\"%g\"  style=\"stroke:rgb(%i,%i,%i);fill:rgb(%i,%i,%i); \"  />",page_width-11.0+sphere_radius,legend_y_offset+8.0*text_line_spacing,1.0,218,165,32,218,165,32);

	fprintf(svgFile, "<text x=\"%g\"  y=\"%g\" >\n ",page_width-9.0,legend_y_offset+8.0*text_line_spacing+text_font_size_mm);

	fprintf(svgFile, " 8 \n");

	fprintf(svgFile, "</text>");



	fprintf(svgFile, "<circle cx=\"%g\" cy=\"%g\" r=\"%g\"  style=\"stroke:rgb(%i,%i,%i);fill:rgb(%i,%i,%i); \"  />",page_width-11.0+sphere_radius,legend_y_offset+9.0*text_line_spacing,1.0,255,215,0,255,215,0);

	fprintf(svgFile, "<text x=\"%g\"  y=\"%g\" >\n ",page_width-9.0,legend_y_offset+9.0*text_line_spacing+text_font_size_mm);

	fprintf(svgFile, " 9 \n");

	fprintf(svgFile, "</text>");




	// now the left hand legend

	sphere_radius = 1.0;


	fprintf(svgFile,"<circle cx=\"%g\" cy=\"%g\" r=\"%g\" style=\" stroke:rgb(0,0,0); stroke-width:%g; fill:rgb(255,255,255);stroke-opacity:1.0;fill-opacity:1.0; \"  />\n",+text_x_offset+sphere_radius,page_height-legend_y_offset,sphere_radius,STROKE_WIDTH*2);

	fprintf(svgFile, "<line x1=\"%g\" y1=\"%g\" x2=\"%g\" y2=\"%g\"  style=\"stroke:black; stroke-width:%g; stroke-opacity:1.0\" /> \n",text_x_offset,page_height-legend_y_offset,+text_x_offset+2.0*sphere_radius,page_height-legend_y_offset,STROKE_WIDTH*2);

	fprintf(svgFile, "<text x=\"%g\"  y=\"%g\" >\n ",+text_x_offset+4*sphere_radius,page_height-legend_y_offset+text_font_size_mm);

	fprintf(svgFile, " Startposition \n");

	fprintf(svgFile, "</text>");



	fprintf(svgFile,"<circle cx=\"%g\" cy=\"%g\" r=\"%g\" style=\" stroke:rgb(0,0,0); stroke-width:%g; fill:rgb(255,255,255);stroke-opacity:1.0;fill-opacity:1.0; \"  />\n", text_x_offset+sphere_radius,page_height-legend_y_offset-1.0*text_line_spacing,sphere_radius,STROKE_WIDTH*2);

	fprintf(svgFile, "<line x1=\"%g\" y1=\"%g\" x2=\"%g\" y2=\"%g\"  style=\"stroke:black; stroke-width:%g; stroke-opacity:1.0\" /> \n",text_x_offset+sphere_radius,page_height-legend_y_offset-1.0*text_line_spacing-sphere_radius,+text_x_offset+sphere_radius,page_height-legend_y_offset-1.0*text_line_spacing+sphere_radius,STROKE_WIDTH*2);

	fprintf(svgFile, "<text x=\"%g\"  y=\"%g\" >\n ",+text_x_offset+4*sphere_radius,page_height-legend_y_offset-1.0*text_line_spacing+text_font_size_mm);

	fprintf(svgFile, "Aktuelle Position \n");

	fprintf(svgFile, "</text>");


	fprintf(svgFile, "<text x=\"%g\"  y=\"%g\" >\n ",+text_x_offset+1*sphere_radius,page_height-legend_y_offset-2.0*text_line_spacing+text_font_size_mm);

	fprintf(svgFile, "time = %5.3g sec \n",mcs.mc_time);

	fprintf(svgFile, "</text>");

	fprintf(svgFile, "<text x=\"%g\"  y=\"%g\" >\n ",+text_x_offset+1*sphere_radius,page_height-legend_y_offset-3.0*text_line_spacing+text_font_size_mm);

	fprintf(svgFile, "T = %5.3g K \n",mcs.temperature);

	fprintf(svgFile, "</text>");









		//fprintf(svgFile, "<text x=\"%g\"  y=\"%g\" >\n ",largest_x_coordinate+text_x_offset,legend_y_offset+text_line_spacing);

		//	fprintf(svgFile, " This is a new line \n");

		//	fprintf(svgFile, "</text>");











	fprintf(svgFile,"</g>\n</defs>\n");

	fprintf(svgFile, "<use xlink:href=\"#write the legend\" transform=\"translate(%g,%g)\" />",sphere_radius,0.0);



















	fprintf(svgFile,"</svg>");

	fclose(svgFile);











}



void data_output_wiki_svg(struct MCS_STRUCT *ptr_to_mcs){

	ptr_to_mcs=&mcs;

	FILE *fhd1;

	int i=0,j=0,actual_i=0,actual_j=0;

	char local_filename[200]="";

	double x_coordinate=0.0,y_coordinate=0.0;


	char fileNamePrefix[200];

	int text_font_size_pts=4;

	double text_font_size_mm=text_font_size_pts*25.4/72.0;

	double text_x_offset=8.0;

	double legend_y_offset=10.0;

	double text_line_spacing=1.5*text_font_size_pts;

	int monte_carlo_step_local=mcs.number_mc_step;







		//  a double column figure in a journal typically has a width of 16 cm
		//  this means 16/2.54 inches
		//  Postscript default resolution 72 points pro inch


	double page_margin=0.1;

	double stroke_width_page_margin=0.3;

	double final_page_width=82.0*3.0;

	double tentative_page_height=final_page_width/(sqrt(2.));


	double relative_sphere_radius=1.0;

	double sphere_radius=tentative_page_height/(2.*relative_sphere_radius+NUMBER_OF_ROWS*sqrt(3)*relative_sphere_radius);


	double measured_page_height=sphere_radius*(2.-sqrt(3)/3.+sqrt(3)*NUMBER_OF_ROWS);

	double measured_page_width=4*sphere_radius+(NUMBER_OF_COLUMNS+NUMBER_OF_ROWS/2.-3./2.)*sphere_radius*2;


	double correctur_factor=measured_page_width/measured_page_height;

	double page_width=final_page_width;

	double page_height=page_width/correctur_factor;


		// the following two x _coordinates (smallest and largest) are used to make a rectangular output instead of a paralleggramm output


    double smallest_x_coordinate=0.0; //=(double)(sphere_radius+((1-1)+(NUMBER_OF_ROWS-1)/2.0)*(sphere_radius*2));

    double largest_x_coordinate=0.0; //(double)(sphere_radius+((NUMBER_OF_COLUMNS-1)+(1-1)/2.0)*(sphere_radius*2));;


		// recalculation of the sphere radius


	sphere_radius=page_height/(2.*relative_sphere_radius+NUMBER_OF_ROWS*sqrt(3)*relative_sphere_radius);


		// Layer C is shifted towards layer B //

		// The shift in x-direction is called delta_x , its value is sphere_radius
		// The shift in x-direction is called delta_y , its value is minus sphere_radius times sqrt(3)/3
		// the later comes from the geometrical calculation that the layer C atom is just in the middle of
		// an equilateral triangle which is spanned by three atoms of the layer B (the layer below C)

	double  delta_x=sphere_radius;

	double  delta_y=sphere_radius*tan(M_PI/6);


	int start_diffusion_i_index=(int)(ceil(NUMBER_OF_ROWS/2.));

	int start_diffusion_j_index=(int)(ceil(NUMBER_OF_COLUMNS/2.));



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



    int red_value=0,green_value=0,blue_value=0;

	int maximum_visits=0,minimum_visits=0,number_of_visits_local=0;



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

		//	sprintf(svg_header_line_three,"<svg  viewBox=\"%g %g %g %g\" width=\"%gmm\" height=\"%gmm\" version=\"1.1\" ",(X_OFFSET_PERCENT/100.*imag_par.page_width),(Y_OFFSET_PERCENT/100.*imag_par.page_height) ,(imag_par.page_width/MAGNIFICATION_IN_VIEW_BOX),(imag_par.page_height/MAGNIFICATION_IN_VIEW_BOX),imag_par.page_width,imag_par.page_height);


	sprintf(svg_header_line_three,"<svg  viewBox=\"0 0 %i %i\" width=\"%gmm\" height=\"%gmm\" version=\"1.1\" ",(int)(ceil(page_width)),(int)(ceil(page_height)),page_width,page_height);

















		// find the maximum and the minimum value of localization times

	for (i=1; i<=NUMBER_OF_ROWS; i++) {
		for (j=1; j<=NUMBER_OF_COLUMNS; j++) {

			if (mcs.number_of_visits_matrix[i][j]>=maximum_visits) {

				maximum_visits=mcs.number_of_visits_matrix[i][j];

			}

			else{   minimum_visits=mcs.number_of_visits_matrix[i][j]; }




		}

	}




		// set the filename


	if (mcs.mc_time>1e-12 && mcs.mc_time<1e-9) {



		sprintf(fileNamePrefix,"random_walk_mc_steps_%07d_temp_%3i_K_time_%03i_pico_sec",mcs.number_mc_step,TEMPERATURE,(int)(mcs.mc_time*1e12));



	}


	if (mcs.mc_time>1e-9 && mcs.mc_time<1e-6) {



		sprintf(fileNamePrefix,"random_walk_mc_steps_%07d_temp_%3i_K_time_%03li_nano_sec",mcs.number_mc_step,TEMPERATURE,lround(mcs.mc_time*1e9));



	}


	if (mcs.mc_time>1e-6 && mcs.mc_time<1e-3) {



		sprintf(fileNamePrefix,"random_walk_mc_steps_%07d_temp_%3i_K_time_%03li_mikro_sec",mcs.number_mc_step,TEMPERATURE,lround(mcs.mc_time*1e6));



	}



	if (mcs.mc_time>1e-3 && mcs.mc_time<1) {



		sprintf(fileNamePrefix,"random_walk_mc_steps_%07d_temp_%3i_K_time_%03g_milli_sec",mcs.number_mc_step,TEMPERATURE,round(mcs.mc_time*1e3));



	}



	if (mcs.mc_time>1 ) {



		sprintf(fileNamePrefix,"random_walk_mc_steps_%07d_temp_%3i_K_time_%03i_sec_%03i_milli_sec",mcs.number_mc_step,TEMPERATURE,(int)(trunc(mcs.mc_time)),(int)((mcs.mc_time-trunc(mcs.mc_time))*1000 ));



	}





	strcat(local_filename,PROGRAMM_DIRECTORY);
	strcat(local_filename,"");
		// fprintf(stdout,"%s\n",local_filename);
	strcat(local_filename,fileNamePrefix);

	strcat(local_filename,".svg");





	fhd1=fopen(local_filename,"w+");


	if (fhd1==NULL) {

		printf("Error in opening the file with name\n -%s- ",local_filename);

		printf("\n\nplease check line number 47 in the programm code \n may be add or remove a / ? \n\n ");


	}

	else {


		printf("\n\n A  svg file  named \n -->%s<-- \nwas successfully written \n ",local_filename);


	}



	fhd1=fopen(local_filename,"w+");

	fprintf(fhd1,"%s\n",svg_header_line_one);

	fprintf(fhd1,"%s\n",svg_header_line_two);

	fprintf(fhd1,"%s\n",svg_header_line_three);

	fprintf(fhd1,"%s\n",svg_header_line_four);

	fprintf(fhd1,"%s\n",svg_header_line_five);


//	fprintf(fhd1," <line x1=\"%g\" y1=\"%g\" x2=\"%g\" y2=\"%g\"  style=\"stroke:red; stroke-width:0.3; stroke-opacity:1.0\" /> \n",page_margin,page_margin, page_width-page_margin,page_height-page_margin);

//	fprintf(fhd1, "<rect x=\"%g\" y=\"%g\" width=\"%i\" height=\"%i\" style=\"fill:none; stroke:black; stroke-width:0.3;stroke-opacity:1.0\" /> \n",page_margin,page_margin,(int)(floor(page_width-1*stroke_width_page_margin)),(int)(page_height-1*stroke_width_page_margin));



    // this double for loop fixes the smallest and largest x-coordinates

    for (i=1; i<=NUMBER_OF_ROWS; i++) {

        for (j=1; j<=NUMBER_OF_COLUMNS; j++){

            x_coordinate=(double)(sphere_radius+((j-1)+(i-1)/2.0)*(sphere_radius*2));
            // the upper line is the shift of each row with index i >1


            y_coordinate=(double)((sphere_radius+1*((i-1)*sphere_radius*sqrt(3))));


            if (i==NUMBER_OF_ROWS && j==1) {



                smallest_x_coordinate=x_coordinate;


            }


            if (i==1 && j==NUMBER_OF_COLUMNS) {

                largest_x_coordinate=x_coordinate;


            }





        }

    }




		// write the substrate layer B


	fprintf(fhd1, "<defs> <g id=\"substrate layer B\">\n");

	fprintf(fhd1, "<style type=\"text/css\"> <![CDATA[  \n");

		// die 64 sind (1-0.75)*255 , 0.75 war im eps output codiert

	fprintf(fhd1,"circle {stroke:rgb(64,64,64); stroke-width:%g; fill:rgb(64,64,64);stroke-opacity:1.0;fill-opacity:1.0 } \n",STROKE_WIDTH);

	fprintf(fhd1, "]]>\n</style>\n  ");



    for (i=1; i<=NUMBER_OF_ROWS; i++) {

        for (j=1; j<=NUMBER_OF_COLUMNS; j++){

            x_coordinate=(double)(sphere_radius+((j-1)+(i-1)/2.0)*(sphere_radius*2));
            // the upper line is the shift of each row with index i >1


            y_coordinate=(double)((sphere_radius+1*((i-1)*sphere_radius*sqrt(3))));

            if (x_coordinate>=smallest_x_coordinate && x_coordinate<=largest_x_coordinate) {


            //    fprintf(stdout,"<circle cx=\"%g\" cy=\"%g\" r=\"%g\" />\n",x_coordinate,y_coordinate,sphere_radius);



                fprintf(fhd1,"<circle cx=\"%g\" cy=\"%g\" r=\"%g\" />\n",x_coordinate,y_coordinate,sphere_radius);


            }

        }

    }


	fprintf(fhd1,"</g>\n</defs>\n");

	fprintf(fhd1, "<use xlink:href=\"#substrate layer B\" transform=\"translate(%g,%g)\" />\n\n",-smallest_x_coordinate+sphere_radius,0.0);





    // write the substrate layer C


    fprintf(fhd1, "<defs> <g id=\"substrate layer C\">\n");

   // fprintf(fhd1, "<style type=\"text/css\"> <![CDATA[  \n");

    // die 128 sind 0.5 *255 , so war das codiert im eps output

  //  fprintf(fhd1,"circle {stroke:rgb(128,128,128); stroke-width:%g; fill:rgb(128,128,128);stroke-opacity:1.0;fill-opacity:1.0 } \n",STROKE_WIDTH);

   // fprintf(fhd1, "]]>\n</style>\n  ");



    for (i=1; i<=NUMBER_OF_ROWS; i++) {

        for (j=1; j<=NUMBER_OF_COLUMNS; j++){


            x_coordinate=(double)(delta_x+sphere_radius+((j-1)+(i-1)/2.0)*(sphere_radius*2));

            y_coordinate=(double) (delta_y+1*(sphere_radius+(i-1)*sphere_radius*sqrt(3)));




            if (x_coordinate>=smallest_x_coordinate && x_coordinate<=largest_x_coordinate) {


                fprintf(fhd1,"<circle cx=\"%g\" cy=\"%g\" r=\"%g\" style=\" stroke:rgb(128,128,128); stroke-width:%g; fill:rgb(128,128,128);stroke-opacity:1.0;fill-opacity:1.0; \"  />\n",x_coordinate,y_coordinate,sphere_radius,STROKE_WIDTH);


           //     fprintf(stdout,"<circle cx=\"%g\" cy=\"%g\" r=\"%g\" />\n",x_coordinate,y_coordinate,sphere_radius);






            }

        }

    }


    fprintf(fhd1,"</g>\n</defs>\n");

    fprintf(fhd1, "<use xlink:href=\"#substrate layer C\" transform=\"translate(%g,%g)\" />\n\n",-smallest_x_coordinate+sphere_radius,0.0);




    // The following double for loops writes the localization_matrix

    // The adsorbate lattice has to be shifted versus substrate layer B
    // So there is 2*delta_x in the x coordinate and 2*delta_y in the y coordinate


    // now  follow the atom on the surface


    fprintf(fhd1, "<defs> <g id=\"follow the atom\">\n");


    for (i=1; i<=NUMBER_OF_ROWS; i++) {

        for (j=1; j<=NUMBER_OF_COLUMNS; j++){

            if(mcs.number_of_visits_matrix[i][j]>0 ){

                x_coordinate=(double) (2*delta_x+sphere_radius+((j-1)+(i-1)/2.0)*(sphere_radius*2));
                // the upper line is the shift of each row with index i >1


                y_coordinate=(double) (2*delta_y+1*(sphere_radius+1*((i-1)*sphere_radius*2*pow(3,0.5)/2.0)));

                number_of_visits_local=mcs.number_of_visits_matrix[i][j];


                if (number_of_visits_local==1) {
                    red_value=0;
                    green_value=0;
                    blue_value=0;

                }

                if (number_of_visits_local==2) {
                    red_value=165;
                    green_value=42;
                    blue_value=42;

                }

                if (number_of_visits_local==3) {
                    red_value=184;
                    green_value=34;
                    blue_value=11;

                }


                if (number_of_visits_local==4) {
                    red_value=255;
                    green_value=64;
                    blue_value=0;

                }

                if (number_of_visits_local==5) {
                    red_value=255;
                    green_value=0;
                    blue_value=0;

                }


                if (number_of_visits_local==5) {
                    red_value=255;
                    green_value=0;
                    blue_value=64;

                }

                if (number_of_visits_local==6) {
                    red_value=255;
                    green_value=64;
                    blue_value=0;

                }

                if (number_of_visits_local==7) {
                    red_value=255;
                    green_value=128;
                    blue_value=0;

                }

                if (number_of_visits_local==8) {
                    red_value=255;
                    green_value=192;
                    blue_value=0;

                }


                if (number_of_visits_local==9) {
                    red_value=255;
                    green_value=255;
                    blue_value=0;

                }

                fprintf(fhd1,"<circle cx=\"%g\" cy=\"%g\" r=\"%g\" style=\" stroke:rgb(%i,%i,%i); stroke-width:%g; fill:rgb(%i,%i,%i);stroke-opacity:1.0;fill-opacity:1.0; \"  />\n",x_coordinate,y_coordinate,sphere_radius,red_value,green_value,blue_value,STROKE_WIDTH,red_value,green_value,blue_value);


				fprintf(fhd1,"<!-- the i j coordinates of the line above are [%i,%i] -->\n>",i,j);




            }

        }

    }

    fprintf(fhd1,"</g>\n</defs>\n");

    fprintf(fhd1, "<use xlink:href=\"#follow the atom\" transform=\"translate(%g,%g)\" />\n\n\n",-smallest_x_coordinate+sphere_radius,0.0);





	 // Finally write the symbol of  the current position.



   fprintf(fhd1, "<defs> <g id=\"write the actual position\">\n");

	actual_i=mcs.i_coordinate_adsorbate;
	actual_j=mcs.j_coordinate_adsorbate;



	x_coordinate=(double)(2*delta_x+sphere_radius+((actual_j-1)+(actual_i-1)/2.0)*(sphere_radius*2));
		// the upper line is the shift of each row with index i >1

	y_coordinate=(double) (2*delta_y+1*(sphere_radius+1*((actual_i-1)*sphere_radius*2*pow(3,0.5)/2.0)));


	fprintf(fhd1,"<circle cx=\"%g\" cy=\"%g\" r=\"%g\" style=\" stroke:rgb(0,0,0); stroke-width:%g; fill:rgb(255,255,255);stroke-opacity:1.0;fill-opacity:1.0; \"  />\n",x_coordinate,y_coordinate,sphere_radius,STROKE_WIDTH);

	fprintf(fhd1, "<!--The atom i j coordinates of the line above are [%i,%i] -->\n",actual_i,actual_j);


	fprintf(fhd1, "<line x1=\"%g\" y1=\"%g\" x2=\"%g\" y2=\"%g\"  style=\"stroke:black; stroke-width:%g; stroke-opacity:1.0\" /> \n",x_coordinate,y_coordinate-sphere_radius,x_coordinate,y_coordinate+sphere_radius,STROKE_WIDTH);


	fprintf(fhd1,"</g>\n</defs>\n");

	fprintf(fhd1, "<use xlink:href=\"#write the actual position\" transform=\"translate(%g,%g)\" />\n\n",-smallest_x_coordinate+sphere_radius,0.0);










    // Finally write the start position symbol in the middle of our adsorbate surfaces. There the single particle starts to diffuse.


    fprintf(fhd1, "<defs> <g id=\"write the start position\">\n");



    x_coordinate=(double)(2*delta_x+sphere_radius+(( start_diffusion_j_index -1)+( start_diffusion_i_index-1)/2.0)*(sphere_radius*2));
    // the upper line is the shift of each row with index i >1

    y_coordinate=(double)(2*delta_y+1*(sphere_radius+1*(start_diffusion_i_index -1)*sphere_radius*2*pow(3,0.5)/2.0));

    fprintf(fhd1,"<circle cx=\"%g\" cy=\"%g\" r=\"%g\" style=\" stroke:rgb(0,0,0); stroke-width:%g; fill:rgb(255,255,255);stroke-opacity:1.0;fill-opacity:1.0; \"  />\n",x_coordinate,y_coordinate,sphere_radius,STROKE_WIDTH);


	fprintf(fhd1, "<!--The atom i j coordinates of the line above are [%i,%i] -->\n",start_diffusion_i_index,start_diffusion_j_index);


    fprintf(fhd1, "<line x1=\"%g\" y1=\"%g\" x2=\"%g\" y2=\"%g\"  style=\"stroke:black; stroke-width:%g; stroke-opacity:1.0\" /> \n",x_coordinate-sphere_radius,y_coordinate,x_coordinate+sphere_radius,y_coordinate,STROKE_WIDTH);



    fprintf(fhd1,"</g>\n</defs>\n");

    fprintf(fhd1, "<use xlink:href=\"#write the start position\" transform=\"translate(%g,%g)\" />",-smallest_x_coordinate+sphere_radius,0.0);














	fprintf(fhd1, "<defs> <g id=\"write the legend\" style=\"font-size:%ipt;font-family:Helvetica\" >\n",text_font_size_pts);


	fprintf(fhd1, "<text x=\"%g\"  y=\"%g\" >\n ",largest_x_coordinate+text_x_offset,legend_y_offset);

	fprintf(fhd1, " Zahl der Aufenthalte \n");

	fprintf(fhd1, "</text>");






	fprintf(fhd1, "<circle cx=\"%g\" cy=\"%g\" r=\"%g\"  style=\"stroke:rgb(%i,%i,%i);fill:rgb(%i,%i,%i); \"  />",largest_x_coordinate+text_x_offset+sphere_radius,legend_y_offset+text_line_spacing,sphere_radius,0,0,0,0,0,0);

	fprintf(fhd1, "<text x=\"%g\"  y=\"%g\" >\n ",largest_x_coordinate+text_x_offset+4*sphere_radius,legend_y_offset+text_line_spacing+text_font_size_mm);

	fprintf(fhd1, " 1 \n");

	fprintf(fhd1, "</text>");


	fprintf(fhd1, "<circle cx=\"%g\" cy=\"%g\" r=\"%g\"  style=\"stroke:rgb(%i,%i,%i);fill:rgb(%i,%i,%i); \"  />",largest_x_coordinate+text_x_offset+sphere_radius,legend_y_offset+2.0*text_line_spacing,sphere_radius,145,42,42,145,42,42);

	fprintf(fhd1, "<text x=\"%g\"  y=\"%g\" >\n ",largest_x_coordinate+text_x_offset+4*sphere_radius,legend_y_offset+2.0*text_line_spacing+text_font_size_mm);

	fprintf(fhd1, " 2 \n");

	fprintf(fhd1, "</text>");



	fprintf(fhd1, "<circle cx=\"%g\" cy=\"%g\" r=\"%g\"  style=\"stroke:rgb(%i,%i,%i);fill:rgb(%i,%i,%i); \"  />",largest_x_coordinate+text_x_offset+sphere_radius,legend_y_offset+3.0*text_line_spacing,sphere_radius,194,34,11,194,34,11);

	fprintf(fhd1, "<text x=\"%g\"  y=\"%g\" >\n ",largest_x_coordinate+text_x_offset+4*sphere_radius,legend_y_offset+3.0*text_line_spacing+text_font_size_mm);

	fprintf(fhd1, " 3 \n");

	fprintf(fhd1, "</text>");



	fprintf(fhd1, "<circle cx=\"%g\" cy=\"%g\" r=\"%g\"  style=\"stroke:rgb(%i,%i,%i);fill:rgb(%i,%i,%i); \"  />",largest_x_coordinate+text_x_offset+sphere_radius,legend_y_offset+4.0*text_line_spacing,sphere_radius,2444,34,11,2444,34,11);

	fprintf(fhd1, "<text x=\"%g\"  y=\"%g\" >\n ",largest_x_coordinate+text_x_offset+4*sphere_radius,legend_y_offset+4.0*text_line_spacing+text_font_size_mm);

	fprintf(fhd1, " 4 \n");

	fprintf(fhd1, "</text>");




	fprintf(fhd1, "<circle cx=\"%g\" cy=\"%g\" r=\"%g\"  style=\"stroke:rgb(%i,%i,%i);fill:rgb(%i,%i,%i); \"  />",largest_x_coordinate+text_x_offset+sphere_radius,legend_y_offset+5.0*text_line_spacing,sphere_radius,255,44,84,255,44,84);

	fprintf(fhd1, "<text x=\"%g\"  y=\"%g\" >\n ",largest_x_coordinate+text_x_offset+4*sphere_radius,legend_y_offset+5.0*text_line_spacing+text_font_size_mm);

	fprintf(fhd1, " 5 \n");

	fprintf(fhd1, "</text>");



	fprintf(fhd1, "<circle cx=\"%g\" cy=\"%g\" r=\"%g\"  style=\"stroke:rgb(%i,%i,%i);fill:rgb(%i,%i,%i); \"  />",largest_x_coordinate+text_x_offset+sphere_radius,legend_y_offset+6.0*text_line_spacing,sphere_radius,255,114,0,255,114,0);

	fprintf(fhd1, "<text x=\"%g\"  y=\"%g\" >\n ",largest_x_coordinate+text_x_offset+4*sphere_radius,legend_y_offset+6.0*text_line_spacing+text_font_size_mm);

	fprintf(fhd1, " 6 \n");

	fprintf(fhd1, "</text>");


	fprintf(fhd1, "<circle cx=\"%g\" cy=\"%g\" r=\"%g\"  style=\"stroke:rgb(%i,%i,%i);fill:rgb(%i,%i,%i); \"  />",largest_x_coordinate+text_x_offset+sphere_radius,legend_y_offset+7.0*text_line_spacing,sphere_radius,255,165,0,255,165,0);

	fprintf(fhd1, "<text x=\"%g\"  y=\"%g\" >\n ",largest_x_coordinate+text_x_offset+4*sphere_radius,legend_y_offset+7.0*text_line_spacing+text_font_size_mm);

	fprintf(fhd1, " 7 \n");

	fprintf(fhd1, "</text>");




	fprintf(fhd1, "<circle cx=\"%g\" cy=\"%g\" r=\"%g\"  style=\"stroke:rgb(%i,%i,%i);fill:rgb(%i,%i,%i); \"  />",largest_x_coordinate+text_x_offset+sphere_radius,legend_y_offset+8.0*text_line_spacing,sphere_radius,218,165,32,218,165,32);

	fprintf(fhd1, "<text x=\"%g\"  y=\"%g\" >\n ",largest_x_coordinate+text_x_offset+4*sphere_radius,legend_y_offset+8.0*text_line_spacing+text_font_size_mm);

	fprintf(fhd1, " 8 \n");

	fprintf(fhd1, "</text>");



	fprintf(fhd1, "<circle cx=\"%g\" cy=\"%g\" r=\"%g\"  style=\"stroke:rgb(%i,%i,%i);fill:rgb(%i,%i,%i); \"  />",largest_x_coordinate+text_x_offset+sphere_radius,legend_y_offset+9.0*text_line_spacing,sphere_radius,255,215,0,255,215,0);

	fprintf(fhd1, "<text x=\"%g\"  y=\"%g\" >\n ",largest_x_coordinate+text_x_offset+4*sphere_radius,legend_y_offset+9.0*text_line_spacing+text_font_size_mm);

	fprintf(fhd1, " 9 \n");

	fprintf(fhd1, "</text>");





	fprintf(fhd1,"<circle cx=\"%g\" cy=\"%g\" r=\"%g\" style=\" stroke:rgb(0,0,0); stroke-width:%g; fill:rgb(255,255,255);stroke-opacity:1.0;fill-opacity:1.0; \"  />\n", largest_x_coordinate+text_x_offset+sphere_radius,legend_y_offset+10.0*text_line_spacing,sphere_radius,STROKE_WIDTH*2);

	fprintf(fhd1, "<line x1=\"%g\" y1=\"%g\" x2=\"%g\" y2=\"%g\"  style=\"stroke:black; stroke-width:%g; stroke-opacity:1.0\" /> \n",largest_x_coordinate+text_x_offset,legend_y_offset+10.0*text_line_spacing,largest_x_coordinate+text_x_offset+2.0*sphere_radius,legend_y_offset+10.0*text_line_spacing,STROKE_WIDTH*2);

	fprintf(fhd1, "<text x=\"%g\"  y=\"%g\" >\n ",largest_x_coordinate+text_x_offset+4*sphere_radius,legend_y_offset+10.0*text_line_spacing+text_font_size_mm);

	fprintf(fhd1, " Startposition \n");

	fprintf(fhd1, "</text>");



	fprintf(fhd1,"<circle cx=\"%g\" cy=\"%g\" r=\"%g\" style=\" stroke:rgb(0,0,0); stroke-width:%g; fill:rgb(255,255,255);stroke-opacity:1.0;fill-opacity:1.0; \"  />\n", largest_x_coordinate+text_x_offset+sphere_radius,legend_y_offset+11.0*text_line_spacing,sphere_radius,STROKE_WIDTH*2);

	fprintf(fhd1, "<line x1=\"%g\" y1=\"%g\" x2=\"%g\" y2=\"%g\"  style=\"stroke:black; stroke-width:%g; stroke-opacity:1.0\" /> \n",largest_x_coordinate+text_x_offset+sphere_radius,legend_y_offset+11.0*text_line_spacing-sphere_radius,largest_x_coordinate+text_x_offset+sphere_radius,legend_y_offset+11.0*text_line_spacing+sphere_radius,STROKE_WIDTH*2);

	fprintf(fhd1, "<text x=\"%g\"  y=\"%g\" >\n ",largest_x_coordinate+text_x_offset+4*sphere_radius,legend_y_offset+11.0*text_line_spacing+text_font_size_mm);

	fprintf(fhd1, "Aktuelle Position \n");

	fprintf(fhd1, "</text>");










		//fprintf(fhd1, "<text x=\"%g\"  y=\"%g\" >\n ",largest_x_coordinate+text_x_offset,legend_y_offset+text_line_spacing);

		//	fprintf(fhd1, " This is a new line \n");

		//	fprintf(fhd1, "</text>");











 fprintf(fhd1,"</g>\n</defs>\n");

	fprintf(fhd1, "<use xlink:href=\"#write the legend\" transform=\"translate(%g,%g)\" />",-smallest_x_coordinate+sphere_radius,0.0);



















	fprintf(fhd1,"</svg>");

	fclose(fhd1);











}





void perform_diffusion(struct MCS_STRUCT *ptr_to_mcs)
{
    ptr_to_mcs=&mcs;

    int diffusion_direction=random_1_6();

    int i_initial=mcs.i_coordinate_adsorbate;
    int j_initial=mcs.j_coordinate_adsorbate;

    int i_final=0;
    int j_final=0;

	int number_mc_steps_local=mcs.number_mc_step;


    if (diffusion_direction==1) {

            // diffusion direction 1
            //  i - >  i and j - > j+1

        i_final=i_periodic(i_initial);
        j_final=j_periodic(j_initial+1);

        mcs.number_of_visits_matrix[i_final][j_final]+=1;;


    }


    if (diffusion_direction==2) {

            // diffusion direction 2
            //  i - > i-1 and j - > j+1


        i_final=i_periodic(i_initial-1);
        j_final=j_periodic(j_initial+1);


        mcs.number_of_visits_matrix[i_final][j_final]+=1;;



    }


    if (diffusion_direction==3) {

            // diffusion direction 3
            //  i - > i-1 and j - > j


        i_final=i_periodic(i_initial-1);
        j_final=j_periodic(j_initial);


        mcs.number_of_visits_matrix[i_final][j_final]+=1;;


         }


    if (diffusion_direction==4) {

            // diffusion direction 4
            //  i - > i and j - > j-1


        i_final=i_periodic(i_initial);
        j_final=j_periodic(j_initial-1);



        mcs.number_of_visits_matrix[i_final][j_final]+=1;;


    }


    if (diffusion_direction==5) {

            // diffusion direction 5
            //  i - > i+1 and j - > j-1


        i_final=i_periodic(i_initial+1);
        j_final=j_periodic(j_initial-1);


        mcs.number_of_visits_matrix[i_final][j_final]+=1;;



    }

    if (diffusion_direction==6) {

            // diffusion direction 6
            //  i - > i+1 and j - > j

        i_final=i_periodic(i_initial+1);
        j_final=j_periodic(j_initial);

        mcs.number_of_visits_matrix[i_final][j_final]+=1;;

        mcs.number_of_visits_matrix[i_initial][j_initial]+=1;


}

    mcs.i_coordinate_adsorbate=i_final;
    mcs.j_coordinate_adsorbate=j_final;
    mcs.mc_time+=mcs.single_adsorbate_diffusion_time;

        //  printf("step %i coordinates %i %i \n",mcs.number_mc_step,i_final,j_final);


}



int main(void)
{
    FILE *csvFile;

    char csvFilename[] = "times.csv";

    csvFile = fopen(csvFilename, "w");
    fprintf(csvFile, "file_number,mc_steps,mc_temperature,mc_time\n");
    fclose(csvFile);

    struct MCS_STRUCT *ptr_to_mcs;
    ptr_to_mcs = &mcs;

    // Use the current time to seed the random number generator
    srand((unsigned int)time(NULL));

    mcs.temperature = TEMPERATURE;
    mcs.index_svg_file = 0;

    int central_i = (int)(round(NUMBER_OF_ROWS / 2));
    int central_j = (int)(round(NUMBER_OF_COLUMNS / 2));

    double *time_values_logarithmic_spacing;

    double single_adsorbate_diffusion_time = 1 / (PREEXPFACTOR_DIFFUSION * exp(-ELECTRON_CHARGE * DIFFUSION_ACTIVATION_ENERGY / (K_BOLTZMANN * TEMPERATURE)));
    mcs.single_adsorbate_diffusion_time = single_adsorbate_diffusion_time;

    fprintf(stdout, "\n\nSingle adsorbate diffusion rate is %g \n\n", 1 / mcs.single_adsorbate_diffusion_time);
    fprintf(stdout, "\n\nSingle adsorbate diffusion time is %g \n\n", mcs.single_adsorbate_diffusion_time);

    int i = 0, j = 0;

    // Allocate memory for the visits matrix
    mcs.number_of_visits_matrix = (int **)calloc((NUMBER_OF_ROWS + 1), sizeof(int *));
    for (i = 0; i <= NUMBER_OF_ROWS; i++) {
        mcs.number_of_visits_matrix[i] = (int *)calloc((NUMBER_OF_COLUMNS + 1), sizeof(int));
    }

    mcs.number_of_visits_matrix[central_i][central_j] = 1;
    mcs.i_coordinate_adsorbate = central_i;
    mcs.j_coordinate_adsorbate = central_j;

    int first_order_of_magnitude_for_output = floor(log10(mcs.single_adsorbate_diffusion_time));
    double time_simulation_finished = 0.0;

    // Allocate memory for the time values array
    time_values_logarithmic_spacing = (double *)calloc(NUMBER_OF_DECADES_IN_TIME * NUMBER_OF_FILES_PER_DECADE_IN_TIME + 1, sizeof(double));

    fprintf(stdout, "You entered %d number of decades in time\nin line 43 of the source code.\n\nYou get %d output files per decade in time\nThis means %d output files in total\n\nfile no --\ttime[sec] \n\n", NUMBER_OF_DECADES_IN_TIME, NUMBER_OF_FILES_PER_DECADE_IN_TIME, NUMBER_OF_DECADES_IN_TIME * NUMBER_OF_FILES_PER_DECADE_IN_TIME + 1);

    for (i = 0; i <= NUMBER_OF_DECADES_IN_TIME * NUMBER_OF_FILES_PER_DECADE_IN_TIME; i++) {
        time_values_logarithmic_spacing[i] = pow(10, first_order_of_magnitude_for_output + i * 1.0 / NUMBER_OF_FILES_PER_DECADE_IN_TIME);
        fprintf(stdout, "file no %i \t\t-- \t %0.03g seconds \n", i + 1, time_values_logarithmic_spacing[i]);
    }

    time_simulation_finished = time_values_logarithmic_spacing[NUMBER_OF_DECADES_IN_TIME * NUMBER_OF_FILES_PER_DECADE_IN_TIME];
    fprintf(stdout, "\nThe simulation is finished at %g seconds\n", time_simulation_finished);

    // Main simulation loop
    for (i = 1; i <= 1e11 && mcs.mc_time < time_simulation_finished; i++) {
        mcs.number_mc_step += 1;
        perform_diffusion(&mcs);

        if (mcs.mc_time > time_values_logarithmic_spacing[j]) {
            j += 1;

            // Output data
            data_output_svg(&mcs);
            fprintf(stdout, " Monte Carlo Time is %3.3g sec at MC step %i \n ", mcs.mc_time, i);
        }
    }

    // Free the allocated memory
    free(time_values_logarithmic_spacing);

    for (i = 0; i <= NUMBER_OF_ROWS; i++) {
        free(mcs.number_of_visits_matrix[i]);
    }
    free(mcs.number_of_visits_matrix);

    return 0;
}















