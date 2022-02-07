# 用户

## 1. 创建用户

```mysql
create user 用户名[@主机名] [identified by '密码']
```

主机名默认值为%，表示这个用户可以从任何主机连接 mysql 服务器。

密码可以省略，表示无密码登录。

## 2. 修改密码

- 通过管理员修改密码

```mysql
set password for '用户名'@'主机'=password('密码');
```

- 通过修改 mysql.user 表修改密码

```mysql
use mysql
update user set authentication_string=password('321') where user = 'test1' and host = '%';
flush privileges;
```

通过表的方式修改之后，需要执行 flush privileges; 才能对用户生效。5.7 中 user 表中的 authentication_string 字段表示密码，旧版本中密码字段是 password。

## 3. 给用户授权

```mysql
grant privileges ON database.table TO 'username'[@'host'] [with grant option]
```

说明

- **privileges**（权限列表），可以是 all，表示所有权限，也可以是 select、update 等权限、多个权限之间用逗号分开
- **ON** 用来指定权限针对哪些库和表，格式为**数据库.表名**，点号前面用来指定数据库名，点号后面用来指定表名，***.\*** 表示所有数据库所有表
- **TO** 表示将权限赋予某个用户，格式为 username@host，@前面为用户名，@后面接限制的主机，可以是IP、IP段、域名以及%，%表示任何地方
- **WITH GRANT OPTION** 这个选项表示该用户可以将自己拥有的权限授权给别人。注意：经常有人在创建操作用户的时候不指定 WITH GRANT OPTION 选项导致后来该用户不能使用 GRANT 命令创建用户或者其它用户授权。备注：可以使用 GRANT 重复给用户添加权限，权限叠加，比如先给用户添加一个 select 权限，然后又给用户添加一个 insert 权限，那么该用户就同时拥有了 select 和 insert 权限。

## 4. 查看用户由哪些权限

```mysql
show grants for '用户名'[@'主机'];
```

主机可以省略，默认值为 %

```mysql
show grants;  // 查看当前用户的权限
```

## 5. 撤销用户的权限

```mysql
revoke privileges ON database.table FROM '用户名'[@'主机'];
```

可以通过 show grants 命令查询一下用户对应的权限，然后使用 revoke 命令撤销用户对应的权限。

## 6. 删除用户

```mysql
drop user '用户名'[@'主机']
```

drop 的方式删除用户之后，用户下次登录就会起效。

```mysql
delete from user where user='用户名' and host='主机';
flush privileges;
```

通过删除 mysql.user 表数据的方式删除，注意通过表的方式删除，需要调用 flush privileges; 刷新权限信息。