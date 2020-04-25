// Maze Demo Main File
// Adapted from original by Joe Wingbermuehle
#include <algorithm>
#include <ctime>
#include <pthread.h>
#include <assert.h>
#include <vector>
#include <thread>
#include <utility>
#include "maze.hpp"
#include "multimap_impl.h"
#include "listener_queue.h"

Multimap <unsigned int, std::vector <unsigned int> > population (10000);

unsigned long futility = 0; 
ThreadSafeListenerQueue <std::vector <unsigned int> > offspring;

void print_genome (std::vector <unsigned int> genome)
{
  std::cout<<"{";
  int i;
  for (i = 0; i < genome.size(); i++)
    std::cout<<genome [i]<<", ";
  std::cout<<"}\n";
}
std::pair <Coord, unsigned int> calc_end_point (Maze m,
                      std::vector <unsigned int> genome) 
{
  Coord current = m.getStart();
  // Traverse the maze and get the ending point
  unsigned int crashes = 0;
  int i;
  for (i = 0; i<genome.size(); i++){
    switch (genome[i]){
      case 0: break;
      case 1: // Turn left
        m.get(current.row, current.col - 1) == 1 ? 
          crashes +=1 : current.col = (current.col - 1); 
        break;
      case 2: // Turn right         

        m.get(current.row, current.col + 1) == 1 ? 
          crashes +=1 : current.col = (current.col + 1); 
        break;
      case 3: // Go down
        m.get(current.row + 1, current.col) == 1 ? 
          crashes +=1 : current.row = (current.row + 1); 
        break;
      case 4: // Go up
        m.get(current.row - 1, current.col) == 1 ? 
          crashes +=1 : current.row = (current.row - 1); 
        break;
      default: std::cout <<"Error: invalid code " << genome[i] <<". Now exitting" <<std::endl;
               std::exit(1);
    }
  }
  return std::pair<Coord, unsigned int> (current, crashes);
}
unsigned int calc_fitness (Coord finish, 
                           Coord current,
                           unsigned int crashes)
{
  unsigned int taxi_cab_dist_x = finish.row < current.row ? 
    (current.row - finish.row) : finish.row - current.row;
  unsigned int taxi_cab_dist_y = finish.col < current.col ? 
    (current.col - finish.col) * -1 : finish.col - current.col;
  unsigned int taxi_cab_dist = 2 * 
    (taxi_cab_dist_x + taxi_cab_dist_y) + crashes;
  return taxi_cab_dist;
}
void mutate (Maze m, 
             unsigned int total_threads)
{

  std::vector <unsigned int> genome;
  std::pair <unsigned int, std::vector <unsigned int> > first_row = 
    population[0];
  std::vector <unsigned int> mutated_genome;
  if (offspring.is_empty()){
  // If all threads run mutate and offspring is empty
    // then it we would get a bug because we listen
    // on offspring to get another element but since
    // no other thread is running mix, offspring will
    // remain empty.
    return;
  }   
  offspring.listen (genome);

  int rand_num = rand () % 10;
  // Only do this 40% of the time
  if (rand_num <=3 ){
    int index = rand () % genome.size();
    int new_val = rand () % 5;
    int i;
    for (i = 0; i < genome.size(); i++) {
      if (i==index) mutated_genome.push_back (new_val);
      else mutated_genome.push_back (genome[i]); 
    }
    std::pair <Coord, unsigned int> result = calc_end_point (m, mutated_genome);

    unsigned int fitness = calc_fitness (m.getFinish(), result.first, result.second);
    population.insert (fitness, 
                       mutated_genome);
    population.truncate (4 * total_threads);
    // if first key in population has decreased
    // set futility to 0
    // Otherwise increment futility
    std::pair <unsigned int, std::vector <unsigned int> > current
      = population[0];
    if (current.first < first_row.first){
      std::cout<<"Fitness: " << fitness <<std::endl;
      futility=0;
    }
    if (fitness == 0){
      // Rudimentary hack to end program when fitness of 0 is
      // reached
      futility += 1000000;
    }
    else futility+=1;
  }
}
void mix ()
{
  // If there are no population elements, we cannot
  // mix
  if (population.get_elements() == 0)
    return;

  unsigned int index_one = rand () % population.get_elements();
  unsigned int index_two = rand () % population.get_elements();

  std::vector <unsigned int> genome_one = 
    population [index_one].second;

  std::vector <unsigned int> genome_two = 
    population [index_two].second;

  int splice_index = (rand() % genome_one.size() - 1) + 1;
  std::vector <unsigned int> new_genome;
  int i;
  for (i = 0; i< splice_index; i++) 
    new_genome.push_back (genome_one [i]);
  for (i = splice_index; i < genome_two.size(); i++)
    new_genome.push_back (genome_two[i]);
  offspring.push (new_genome);
}
// Initializes first genomes and puts them into population
void init_genomes (Maze m,
                   unsigned int number_of_genomes, 
                   unsigned int genome_size)
{
  int i,j;
  for (i = 0; i<number_of_genomes; i++){
    std::vector <unsigned int> tmp_genome;
    for (j = 0; j<genome_size; j++){
      tmp_genome.push_back (rand() % 5);
    }
    std::pair <Coord, unsigned int> result = calc_end_point (m, tmp_genome);
    unsigned int fitness = calc_fitness (m.getFinish(), result.first, result.second);
    population.insert (fitness, 
                       tmp_genome);
  }
}
/** Generate and display a random maze. */
int main(int argc, char* argv[]) {
	std::srand(std::time(0));			// Remember to do this!

  assert (argc == 6);
  unsigned int total_threads = atoi(argv[1]);
  unsigned int g = atoi (argv[2]);
  unsigned int rows = atoi (argv[3]);
  unsigned int cols = atoi (argv[4]);
  unsigned int genome_length = atoi (argv[5]);
  assert (total_threads >= 1);
  assert (rows >= 3);
  assert (cols >= 3);
  assert (g > 0);
  assert (genome_length > 0);
  std::cout<<std::endl<<std::endl<<"Threads: " << total_threads << " g: " << g << " rows: " << rows << " cols: "<<cols<<" genome_length: " << genome_length<<std::endl;
  std::clock_t start;
  float duration;
  start = std::clock();



	Maze m(rows, cols);
	std::cout << m;
  // Prints out maze in 1s and 0s
  
  int i;


 	std::cout << "Start at (" << m.getStart().row << ", " << m.getStart().col << ")" << std::endl;
	std::cout << "Finish at (" << m.getFinish().row << ", " << m.getFinish().col << ")" << std::endl;
   
  init_genomes (m, total_threads * 4, genome_length);
  std::cout<<"Genome initialized\n";
  ThreadSafeListenerQueue <std::thread*> threads;
  while (futility < g){
    // Initialize all threads. Half are mixer threads
    // and half are mutator threads
    for (i = 0; i < total_threads; i++){
      unsigned int rand_num = rand() % 5;
//      std::cout<<"Rand_num = " << rand_num <<std::endl;
      if (rand_num >= 3){
        threads.push (new std::thread (mutate, m, total_threads));
      }
      else {
        threads.push (new std::thread (mix));
      }
    }

    // Join all threads
    for (i = 0; i< total_threads; i++){
      std::thread* t;
      threads.pop (t);
      t -> join();
    }
  }
  std::pair <unsigned int, std::vector <unsigned int> > top_genome = population [0];
    std::pair <Coord, unsigned int> result = calc_end_point (m, top_genome.second);

  duration = (std::clock() - start) / (float) CLOCKS_PER_SEC;
  std::cout<<"------------------------\n";
  std::cout<<"Fitness: " << top_genome.first<<std::endl;
  std::cout <<"End point: (" << result.first.row << ", " << result.first.col << ")" << std::endl;
  std::cout<<"Total time: " << duration <<std::endl;
  std::cout<<"Top genome: ";
  print_genome (top_genome.second);
	return 0;
}
