#include "Config.hpp"

int	main(int argc, char **argv)
{
	/*** PARSING ***/
	Config	conf(argc >= 2 ? argv[1] : DEFAULT_CONF);

	/*** DEBUG ***/
	for (size_t j = 0; j < conf.servers.size(); j++)
		conf.servers[j].debug();
	std::cout << std::endl;

	/*** LAUNCH ***/
	pthread_t *threads = new pthread_t[conf.servers.size()];
	for (size_t i = 0; i < conf.servers.size(); i++)
		pthread_create(threads + i, NULL, (void* (*)(void *))Server::start, (void *)&conf.servers[i]);

	/*** JOIN ***/
	for (size_t i = 0; i < conf.servers.size(); i++)
		pthread_join(threads[i], NULL);

	/*** FREE ***/
	delete [] threads;
}