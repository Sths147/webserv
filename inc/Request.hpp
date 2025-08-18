/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sithomas <sithomas@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/04 10:20:10 by sithomas          #+#    #+#             */
/*   Updated: 2025/08/06 11:57:08 by sithomas         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "header.hpp"

class Request
{
    private:
    int                                             _return_code;
        bool                                        _finished;
        const std::string                           _type;
        const std::string                           _target;
        const std::string                           _http_type;
        const std::map<std::string, std::string>    _header;
        const std::vector<char>                     _body;
    
    public:
        Request();
        Request(std::vector<char>&);
        ~Request();
        // Request&    operator=(const Request&);
        std::string     get_type();
        std::string     get_target();
        std::string     get_http_version();
        // void            print_headers();
        // void            print_body();
};