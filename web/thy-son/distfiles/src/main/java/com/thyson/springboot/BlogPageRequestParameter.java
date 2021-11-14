package com.thyson.springboot;

import com.fasterxml.jackson.annotation.JsonCreator;
import com.fasterxml.jackson.annotation.JsonProperty;

public class BlogPageRequestParameter {
  @JsonProperty("id")
  public final String id;

  @JsonProperty("lang")
  public final String lang;

  @JsonCreator
  public BlogPageRequestParameter(@JsonProperty("id") String id, @JsonProperty("lang") String lang) {
    int idx = Integer.parseInt(id, 10);
    if (idx < 0 || 3 < idx) {
      throw new IllegalArgumentException();
    }

    this.id = id;
    this.lang = lang;
  }
}
