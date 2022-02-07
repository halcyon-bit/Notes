# 基本命令

## 1. 登录命令

```mysql
mysql -h ip -P 端口 -u 用户名 -p
```

## 2. 查看数据库版本

```mysql
// 未登录状态下
mysql -V 或 mysql --version
// 登录后
select version();
```

## 3. 显示所有数据库

```mysql
show databases;
```

## 4. 进入指定的库

```mysql
use 库名;
```

## 5. 显示当前库中所有表

```mysql
show tables;
```

## 6. 显示表列

```mysql
show columns from 表名
```

## 7. 显示其他库中所有表

```mysql
show tables from 库名;
```

## 8. 查看表的创建语句

```mysql
show create table 表名;
```

## 9. 查看表结构

```mysql
desc 表名;
```

## 10. 查看当前所在库

```mysql
select database();
```

## 11. 查看当前 mysql 支持的存储引擎

```mysql
SHOW ENGINES;
```

## 12. 查看系统变量及其值

```mysql
SHOW VARIABLES;
```

## 13. 数据类型

- 整数类型：bit bool tinyint smallint mediumint int bigint
- 浮点数类型：float double decimal
- 字符串类型：char varchar tinyblob blob mediumblob longblob tinytext text mediumtext longtext
- 日期类型：Date DateTime TimeStamp Time Year