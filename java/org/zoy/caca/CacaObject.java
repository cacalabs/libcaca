/**
 *  libcaca       Java bindings for libcaca
 *  Copyright (c) 2009 Adrien Grand <jpountz@dinauz.org>
 *
 *  This library is free software. It comes without any warranty, to
 *  the extent permitted by applicable law. You can redistribute it
 *  and/or modify it under the terms of the Do What the Fuck You Want
 *  to Public License, Version 2, as published by Sam Hocevar. See
 *  http://www.wtfpl.net/ for more details.
 */

package org.zoy.caca;

public abstract class CacaObject {

  protected String code;
  protected String description;
  public CacaObject(String code, String desc) {
    this.code = code;
    this.description = desc;
  }
  public String getCode() {
    return code;
  }
  public String getDescription() {
    return description;
  }
  @Override
  public boolean equals(Object o) {
    if (o == null) return false;
    if (this.getClass().equals(o.getClass())) {
      CacaObject other = (CacaObject)o;
      return this.code.equals(other.code);
    }
    return false;
  }

}
