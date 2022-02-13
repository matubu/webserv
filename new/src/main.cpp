#include "Config.hpp"

Config	*conf;

void	cleanup()
{
	for (size_t i = 0; i < conf->servers.size(); i++)
		conf->servers[i].closeSocket();
}
void	handler(int sig)
{
	(void)sig;
	exit(1);
}

int	main(int argc, char **argv)
{
	/*** PARSING ***/
	Config	lconf(argc >= 2 ? argv[1] : DEFAULT_CONF);
	conf = &lconf;

	atexit(cleanup);
	signal(SIGINT, handler);

	/*** DEBUG ***/
	for (size_t j = 0; j < conf->servers.size(); j++)
		conf->servers[j].debug();
	std::cout << std::endl;

	/*** LAUNCH ***/
	pthread_t *threads = new pthread_t[conf->servers.size()];
	for (size_t i = 0; i < conf->servers.size(); i++)
		pthread_create(threads + i, NULL, (void* (*)(void *))Server::start, (void *)&conf->servers[i]);

	/*** JOIN ***/
	for (size_t i = 0; i < conf->servers.size(); i++)
		pthread_join(threads[i], NULL);

	/*** FREE ***/
	delete [] threads;
}