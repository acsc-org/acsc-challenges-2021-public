package com.thyson.springboot;

import org.springframework.context.annotation.Configuration;
import org.springframework.context.annotation.Bean;

import org.springframework.security.config.annotation.web.builders.HttpSecurity;
import org.springframework.security.config.annotation.web.configuration.EnableWebSecurity;
import org.springframework.security.config.annotation.web.configuration.WebSecurityConfigurerAdapter;
import org.springframework.security.config.annotation.authentication.builders.AuthenticationManagerBuilder;

import org.springframework.security.config.http.SessionCreationPolicy;

import org.springframework.security.crypto.password.PasswordEncoder;
import org.springframework.security.crypto.bcrypt.BCryptPasswordEncoder;

@Configuration
@EnableWebSecurity
public class BasicAuthConfig extends WebSecurityConfigurerAdapter {

  @Override
  protected void configure(AuthenticationManagerBuilder auth) throws Exception {
    String user = System.getenv("CS_USERNAME");
    String pass;
    if (user == null || user.isEmpty()) {
      user = "admin";
      pass = "admin";
    } else {
      pass = System.getenv("CS_PASSWORD");
    }

    PasswordEncoder encoder = passwordEncoder();

    auth.inMemoryAuthentication()
      .passwordEncoder(encoder)
      .withUser(user)
      .password(encoder.encode(pass))
      .authorities("ROLE_USER");
  }

  @Override
  protected void configure(HttpSecurity http) throws Exception {
    http.httpBasic()
      .and()
        .authorizeRequests().anyRequest().authenticated();
    http.csrf().disable();
  }

  @Bean
  public PasswordEncoder passwordEncoder() {
    return new BCryptPasswordEncoder();
  }
}
