package com.thyson.springboot;

import java.util.Map;

import java.io.InputStream;
import java.io.IOException;

import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;
import javax.servlet.http.Cookie;

import org.springframework.ui.Model;

import org.springframework.http.MediaType;
import org.springframework.http.HttpInputMessage;
import org.springframework.http.converter.json.MappingJackson2HttpMessageConverter;
import org.springframework.http.server.ServletServerHttpRequest;

import org.springframework.stereotype.Controller;

import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RequestBody;
import org.springframework.web.bind.annotation.RequestParam;
import org.springframework.web.bind.annotation.RequestMethod;
import org.springframework.web.bind.annotation.CookieValue;

import org.springframework.beans.factory.annotation.Autowired;

import com.fasterxml.jackson.databind.ObjectMapper;
import com.fasterxml.jackson.core.JsonProcessingException;

@Controller
public class ThysonController {
    @Autowired
    private MappingJackson2HttpMessageConverter mappingJackson2HttpMessageConverter;


    @RequestMapping("/")
    public String index() {
        return "index";
    }

    @RequestMapping(
        value = "/login", 
        method = {RequestMethod.GET, RequestMethod.POST}
    )
    public String login(HttpServletResponse response, HttpServletRequest request, @RequestParam(required = false) Map<String, String> map, Model model) throws IOException, JsonProcessingException {
        HttpInputMessage inputMessage = new ServletServerHttpRequest(request) {
          @Override
            public InputStream getBody() throws IOException {
              return request.getInputStream();
            }
        };

        LoginPageRequestParameter param = null;
        if (request.getMethod().equals("GET") && !request.getParameterNames().hasMoreElements()) {
            return "login";
        }

        if (request.getMethod().equals("POST") && request.getContentType().equals(MediaType.APPLICATION_JSON_VALUE)) {
            param = (LoginPageRequestParameter) mappingJackson2HttpMessageConverter.read(LoginPageRequestParameter.class, inputMessage);
        } else {
            ObjectMapper mapper = new ObjectMapper();
            param = mapper.convertValue(map, LoginPageRequestParameter.class);
        }

        Cookie cookie = new Cookie("username", param.username);
        cookie.setMaxAge(365 * 24 * 60 * 60); 
        cookie.setPath("/"); 
        response.addCookie(cookie);

        return "redirect:/blog?id=1&lang=en";
    }

    @RequestMapping(
        value = "/blog", 
        method = {RequestMethod.GET, RequestMethod.POST}
    )
    public String blog(HttpServletRequest request, @RequestParam(required = false) Map<String, String> map, Model model, @CookieValue(value="username", required=false) String username) throws IOException, JsonProcessingException {
        HttpInputMessage inputMessage = new ServletServerHttpRequest(request) {
          @Override
            public InputStream getBody() throws IOException {
              return request.getInputStream();
            }
        };

        BlogPageRequestParameter param = null;
        if (request.getMethod().equals("POST") && request.getContentType().equals(MediaType.APPLICATION_JSON_VALUE)) {
            param = (BlogPageRequestParameter) mappingJackson2HttpMessageConverter.read(BlogPageRequestParameter.class, inputMessage);
        } else {
            ObjectMapper mapper = new ObjectMapper();
            param = mapper.convertValue(map, BlogPageRequestParameter.class);
        }

        if (!param.lang.equals("en") && !param.lang.equals("fr")) {
            // not supported!!
            throw new IllegalArgumentException();
        }

        model.addAttribute("username", username);
        model.addAttribute("id", param.id);
        model.addAttribute("lang", param.lang);

        if (param.id.equals("1")) {
            model.addAttribute("nextid", "2");
        } else if(param.id.equals("2")) {
            model.addAttribute("previd", "1");
            model.addAttribute("nextid", "3");
        } else if(param.id.equals("3")) {
            model.addAttribute("previd", "2");
        }

        return "blog";
    }
}
