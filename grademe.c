#include <mysql/mysql.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>
#include <libgen.h>
#ifdef __APPLE__
#include <mach-o/dyld.h>
#endif

typedef struct grade
{
    int grade;
}t_grade;

void print_time_left(int timer)
{
    int four_hour = 14400;
    int time_left = four_hour - timer;
    int hours = time_left / 3600;
    int rem = time_left % 3600;
    int minute = rem / 60;
    int seconde = rem % 60;
    printf("   🦄 Time Left : [ \033[35m %dh%dm%ds \033[0m ]\n",hours,minute,seconde);
}

void debug_grademe_case(char *expected,char *output)
{
    printf("\n\n****** DEBUG *******\n\nGot %s\n\nExepected %s\n",output,expected);
}

void init_render_traces()
{
    char p[1024]; unsigned size = sizeof(p);

    int os = 0;

#ifdef __APPLE__
    _NSGetExecutablePath(p, &size);
    os = 2;
#else
    int n = readlink("/proc/self/exe", p, sizeof(p)-1);
    p[n] = 0;
    os = 1;
#endif

    if(os == 2)
    {
        printf("\nOS : MAC\n");
        char cmd[2048];
        sprintf(cmd,
        "rm -rf \"%s/render\" \"%s/traces\" && mkdir \"%s/render\" \"%s/traces\"",
        dirname(p), dirname(p), dirname(p), dirname(p));
        system(cmd);

        //Not good way its fine for the moment
        char buff_path[300];
        sprintf(buff_path,"cd %s/traces && touch trace.txt",dirname(p));
        system(buff_path);
    }

    if(os == 1)
    {
    const char *home = getenv("HOME");
    char cmd[512];
    char buff_path[512];

    sprintf(cmd, "mkdir -p \"%s/grademe/render\" \"%s/grademe/traces\"", home, home);
    system(cmd);

    sprintf(buff_path, "touch %s/grademe/traces/trace.txt", home);
    system(buff_path);
    }
    
}

void commande()
{
    printf("\n(grademe) to send you assignement");
    printf("\n(status) to see you current status and your assignement");
    printf("\n(finish) to end the exam\n\n");
}

void welcomer()
{
    printf("Welcome into Grademe v0.0.0\n\n");
    printf("Want to Start a Exam ?\n\n(1) Exam_1🚫\n(2) Exam_2✅\n(3) Exam_3🚫\n(4) Exam_4🚫\n\n");
}

int word_to_ascii(char *word)
{
    int i = 0;
    int ascii = 0;
    while(word[i])
    {
        ascii += word[i];
        i++;
    }
    return (ascii);
}

char **fetche_disigned_data(MYSQL *conn, char *target_id)
{
    MYSQL_RES *res;
    MYSQL_ROW row;

    char query[200];
    snprintf(query, sizeof(query), "SELECT * FROM outcase WHERE id = %s", target_id);

    if (mysql_query(conn, query)) {
        printf("error\n");
        mysql_close(conn);
        return NULL;
    }
    else
    {
        printf("\n \033[0;32m[query]\033[0m\n");
    }

    res = mysql_store_result(conn);
    if (!res) {
        printf("empty\n");
        return NULL;
    }

    int num = mysql_num_fields(res);
    row = mysql_fetch_row(res);

    char **finals = malloc(num * sizeof(char *));

    for (int i = 0; i < num; i++) {
        finals[i] = malloc(900 * sizeof(char));
        if (row[i]) {
            strncpy(finals[i], row[i], 899);
            finals[i][899] = '\0';
        } else {
            finals[i][0] = '\0';
        }
    }

    mysql_free_result(res);
    return finals;
}

char **gen_configs() {
    static char *configs[10][10] = {
        {"2","4","6","8","11","13","18","20","25","28"},
        {"1","3","5","7","10","14","16","22","27","30"},
        {"3","5","6","9","12","15","20","23","28","30"},
        {"2","4","7","8","11","13","17","21","26","29"},
        {"1","3","5","7","10","12","18","22","25","30"},
        {"3","4","6","9","12","14","19","23","27","30"},
        {"2","5","6","8","11","15","16","20","24","28"},
        {"1","3","5","7","10","12","17","21","26","30"},
        {"2","4","6","8","12","14","18","22","27","30"},
        {"1","3","5","9","11","15","19","23","26","30"}
    };

    int nombre = rand() % 9; // 0 to 8
    return configs[nombre];
}

void print_status(t_grade *current_grade,char **str,time_t debut)
{
    printf("\n\n🦄Exercice name : %s.             File To Turn in \033[0;32m./render/%s.c\033[0m\n   \n",str[1],str[1]);
    print_time_left(time(NULL) - debut);
    printf("                                          \033[0;35mCurrent\033[0m Grade : 🦄[ %d/100 ]\n",current_grade->grade);
    printf("\n%s\n",str[2]);
}

void print_succes(t_grade *current_grade,char **str)
{
    printf("\n\n >>>>>>>> \033[0;32mSUCCES\033[0m <<<<<<<<\n\n");
    printf("            Current Grade : %d/100\n\n",current_grade->grade);
    commande();
}

void print_fail(t_grade *current_grade,char **str)
{
    printf("\n\n >>>>>>>> \033[0;31mFAILURE\033[0m <<<<<<<<\n\n");
    printf("            Current Grade : %d/100\n\n",current_grade->grade);
    commande();
}

int existe_file(char *file_test)
{

                char p[1024]; unsigned size = sizeof(p);
#ifdef __APPLE__
    _NSGetExecutablePath(p, &size);
#else
    int n = readlink("/proc/self/exe", p, sizeof(p)-1);
    p[n] = 0;
#endif

    char dest_path[1000];
    sprintf(dest_path,"%s/render/%s.c",dirname(p),file_test);
    FILE *tt = fopen(dest_path,"r");
    if(tt == NULL)
    {
        return (0);
    }
    return (1);
}

void rm_content(char *main_tester, char *file_name, char *lib_std)
{

            char p[1024]; unsigned size = sizeof(p);
#ifdef __APPLE__
    _NSGetExecutablePath(p, &size);
#else
    int n = readlink("/proc/self/exe", p, sizeof(p)-1);
    p[n] = 0;
#endif

    char lib_test[19] = "#include <stdio.h>";
    char buff[500];
    sprintf(buff,"%s/render/%s.c",dirname(p),file_name);
    char buff2[500];
    sprintf(buff2,"%s/render/temp.txt",dirname(p));
    FILE *read_file = fopen(buff,"r");
    FILE *temp = (fopen(buff2,"w"));

    char line[1000];
    while(fgets(line,sizeof(line),read_file))
    {
        if(!(strcmp(line,main_tester) == 0 ||strcmp(line,lib_std) == 10))
        {
            fputs(line,temp);
        }
    }
    //char buff3[300];
    //sprintf(buff3,"%s.c",file_name);

    rename(buff2,buff);

    fclose(temp);
    fclose(read_file);
}

int tester_func(char *main_tester, char *expected_case,char *file_name,int is_file_existe)
{

        char p[1024]; unsigned size = sizeof(p);
#ifdef __APPLE__
    _NSGetExecutablePath(p, &size);
#else
    int n = readlink("/proc/self/exe", p, sizeof(p)-1);
    p[n] = 0;
#endif

    //printf("\n%s\n",expected_case);
    //Add int main()
    if(is_file_existe == 0)
    {
        char buff_traces_path[400];
        sprintf(buff_traces_path,"%s/traces/trace.txt",dirname(p));
        FILE *report_traces = fopen(buff_traces_path,"a");
        fputs("\nError file not provided in './render' please create your assignement\n",report_traces);
        fclose(report_traces);
        return (0);
    }
    char exer_path[1000];
    sprintf(exer_path,"%s/render/%s.c",dirname(p),file_name);
    FILE *add_mainT = fopen(exer_path,"a+");
    char tester[1300];
    sprintf(tester,"\n%s",main_tester);
    FILE *add_test_lib = fopen(exer_path,"a");
    fputs("\n#include <stdio.h>",add_test_lib);
    fclose(add_test_lib);
    fputs(tester,add_mainT);
    fclose(add_mainT);

    /*
    compile compare
    */
    char path_ex_and_compile[1000];
    sprintf(path_ex_and_compile,"gcc -o %s/test_ex %s/render/%s.c 2>%s/traces/trace.txt",dirname(p),dirname(p),file_name,dirname(p));
    system(path_ex_and_compile);

    char line[4000];
    char path_exec[250];
    char result[1000] = "";
    sprintf(path_exec,"%s/test_ex",dirname(p));
    FILE *open_exec = popen(path_exec,"r");

    while(fgets(line,sizeof(line),open_exec))
    {
        strcat(result,line);
    }

    //debug_grademe_case(result,expected_case);

    pclose(open_exec);
    //sleep(1);
    rm_content(main_tester,file_name,"#include <stdio.h>");

    if(strcmp(result,expected_case) == 0)
    {
        return (1);
    }
    //compile File here catche result compare top of rm_content
    
    return (0);
}

int main ()
{
    MYSQL *conn;

    conn = mysql_init(NULL);

    char host[64], user[64], pass[64], db[64];
    unsigned int port;

    FILE *f = fopen("config.ini", "r");

    fscanf(f, "host=%63s\nuser=%63s\npassword=%63s\ndatabase=%63s\nport=%u", 
           host, user, pass, db, &port);
    fclose(f);

    if(mysql_real_connect(conn, host,user,pass,db,port,NULL,CLIENT_SSL) != 0)
    {
        printf("🦄Connected Mysql V8.0🦄\n\n");
        welcomer();
    }
    else
    {
        printf("ERROR");
        return (0);
    }
    t_grade user_grade;
    user_grade.grade = 0;

    int Exam_mode = 5;
    while(Exam_mode > 4)
    {
        printf("[🦄🦄🦄] > ");
        scanf("%d",&Exam_mode);

        if(Exam_mode == 1)
        {
            printf("\n🦄Not avaliable\n");
            return (0);
        }
        if(Exam_mode == 2)
        {

            int secondes = 14400;
            time_t debut = time(NULL);
            

            init_render_traces();
             //init render; NEXT STEP
            char **configs = gen_configs();
            printf("\n        🦄You enter in exam mode (Started : [Exam %d])\n",Exam_mode);
            commande();
            char action[50];
            int ascii_action = 0;
            int skipper = 0;
            while(time(NULL) - debut < secondes && ascii_action != 641)
            {
                printf("[🦄🦄🦄] > ");
                scanf("%s",action);
                ascii_action = word_to_ascii(action);
                if(ascii_action == 676) // status and get current assignement
                {
                    char **fetch_data = fetche_disigned_data(conn,configs[skipper]);
                    print_status(&user_grade,fetch_data,debut);
                    commande();
                }
                if(ascii_action == 725) // Grademe
                {
                    //Check if at first file existe !
                    char **fetch_data = fetche_disigned_data(conn,configs[skipper]);
                    //add int main compile test delete int main !; fetch_data[3] -> is the int main [4] test_case;
                    if(tester_func(fetch_data[3],fetch_data[4],fetch_data[1],existe_file(fetch_data[1])) == 0)
                    {
                        print_fail(&user_grade,fetch_data);
                    }
                    else
                    {
                        skipper++;
                        user_grade.grade += 10;
                        //Succes msg;
                        print_succes(&user_grade,fetch_data);
                    }

                }
            }
        }
        if(Exam_mode == 3)
        {
            printf("\n🦄Not avaliable\n");
        }
        if(Exam_mode == 4)
        {
            printf("\n🦄Not avaliable\n");
        }
    }



    /*
    char **assignement = gen_configs();
    char **finals = fetche_disigned_data(conn,assignement[9]);
    printf("%s",finals[1]);
    printf("%s",finals[2]);
    printf("%s",finals[3]);
    printf("%s",finals[4]);
    */
}