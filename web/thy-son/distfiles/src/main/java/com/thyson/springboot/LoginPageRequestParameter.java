package com.thyson.springboot;

import com.fasterxml.jackson.annotation.JsonCreator;
import com.fasterxml.jackson.annotation.JsonProperty;

public class LoginPageRequestParameter {
  @JsonProperty("username")
  public final String username;

  @JsonProperty("password")
  public final String password;

  @JsonProperty("birth")
  public final String birth;

  @JsonCreator
  public LoginPageRequestParameter(@JsonProperty("username") String username, @JsonProperty("password") String password, @JsonProperty("birth") String birth) {
    this.username = username;
    this.password = password;

    if (birth.length() != 10) {
        throw new IllegalArgumentException();
    }

    if (birth.charAt(2) != '/' || birth.charAt(5) != '/') {
        throw new IllegalArgumentException();
    }

    int day = Integer.parseInt(birth.substring(0, 2), 10);
    if (day < 1 || 31 < day) {
        throw new IllegalArgumentException();
    }

    int month = Integer.parseInt(birth.substring(3, 5), 10);
    if (month < 1 || 12 < month) {
        throw new IllegalArgumentException();
    }

    int year = Integer.parseInt(birth.substring(6, 10), 10);
    if (year < 1 || 2021 < year) {
        throw new IllegalArgumentException();
    }

    this.birth = birth;
  }
}
