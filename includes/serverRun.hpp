/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   serverRun.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rertzer <rertzer@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/12 09:39:40 by rertzer           #+#    #+#             */
/*   Updated: 2023/09/13 11:47:09 by rertzer          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVERRUN_HPP
# define SERVERRUN_HPP

# include <unistd.h>
# include <stdlib.h>
# include <string>
# include <sstream>
# include <vector>
# include <map>
# include <sys/epoll.h>
# include <iostream>

# include "Polling.hpp"
# include "Server.hpp"
# include "Status.hpp"
# include "ErrorException.hpp"

typedef void	(*handlestatus)(Event &ev, Polling & pool);

void	handleEvent(Polling & pool, std::vector<Server> & serv);
void	loadMotherSocket(Polling & pool, std::vector<Server> serv);
void	eventOnMother(Event & ev, Polling & pool);
void	eventOnOther(Event & ev, Polling & pool);
void	checkBadEventOnMother(Event & ev, Polling & pool);
void	handleEventStatus(Event & ev, Polling & pool);
void	handleInOk(Event & ev, Polling & pool);
void	handleOutOk(Event & ev, Polling & pool);
void	handleClose(Event & ev, Polling & pool);
void	handleCgiPostStart(Event & ev, Polling & pool);
void	handleCgiContinue(Event & ev, Polling & pool);
void	handleCgiEnd(Event & ev, Polling & pool);
void	handleCgiPostExec(Event & ev, Polling & pool);
void	handleCgiGetExec(Event & ev, Polling & pool);
void	handleCgiError(Event & ev, Polling & pool);

#endif
