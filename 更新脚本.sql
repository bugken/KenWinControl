/*
更新内容
1.增加表 tab_Game_Control
2.增加表 tab_Game_Control_Log
3.增加表 tab_Game_All_SelectType
4.增加表 tab_Game_Result
*/
---------------------------------------增加 tab_Game_Control--------------------------------------------------------------------------------------------
USE [caipiaos]
GO

IF  EXISTS (SELECT * FROM dbo.sysobjects WHERE id = OBJECT_ID(N'[DF__tab_Game___UserC__035C66C6]') AND type = 'D')
BEGIN
ALTER TABLE [dbo].[tab_Game_Control] DROP CONSTRAINT [DF__tab_Game___UserC__035C66C6]
END

GO

IF  EXISTS (SELECT * FROM dbo.sysobjects WHERE id = OBJECT_ID(N'[DF__tab_Game___Contr__04508AFF]') AND type = 'D')
BEGIN
ALTER TABLE [dbo].[tab_Game_Control] DROP CONSTRAINT [DF__tab_Game___Contr__04508AFF]
END

GO

IF  EXISTS (SELECT * FROM dbo.sysobjects WHERE id = OBJECT_ID(N'[DF__tab_Game___Perio__0544AF38]') AND type = 'D')
BEGIN
ALTER TABLE [dbo].[tab_Game_Control] DROP CONSTRAINT [DF__tab_Game___Perio__0544AF38]
END

GO

IF  EXISTS (SELECT * FROM dbo.sysobjects WHERE id = OBJECT_ID(N'[DF__tab_Game___Power__072CF7AA]') AND type = 'D')
BEGIN
ALTER TABLE [dbo].[tab_Game_Control] DROP CONSTRAINT [DF__tab_Game___Power__072CF7AA]
END

GO

IF  EXISTS (SELECT * FROM dbo.sysobjects WHERE id = OBJECT_ID(N'[DF__tab_Game___Updat__0638D371]') AND type = 'D')
BEGIN
ALTER TABLE [dbo].[tab_Game_Control] DROP CONSTRAINT [DF__tab_Game___Updat__0638D371]
END

GO

USE [caipiaos]
GO

/****** Object:  Table [dbo].[tab_Game_Control]    Script Date: 09/01/2020 19:22:36 ******/
IF  EXISTS (SELECT * FROM sys.objects WHERE object_id = OBJECT_ID(N'[dbo].[tab_Game_Control]') AND type in (N'U'))
DROP TABLE [dbo].[tab_Game_Control]
GO

USE [caipiaos]
GO

/****** Object:  Table [dbo].[tab_Game_Control]    Script Date: 09/01/2020 19:22:38 ******/
SET ANSI_NULLS ON
GO

SET QUOTED_IDENTIFIER ON
GO

CREATE TABLE [dbo].[tab_Game_Control](
	[GID] [bigint] IDENTITY(1,1) NOT FOR REPLICATION NOT NULL,
	[UserControled] [int] NULL,
	[ControlRate] [int] NULL,
	[PeriodGap] [int] NULL,
	[PowerControl] [int] NULL,
	[UpdateTime] [datetime2](7) NULL,
PRIMARY KEY CLUSTERED 
(
	[GID] ASC
)WITH (PAD_INDEX  = OFF, STATISTICS_NORECOMPUTE  = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS  = ON, ALLOW_PAGE_LOCKS  = ON) ON [PRIMARY]
) ON [PRIMARY]

GO

EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'自增ID' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'tab_Game_Control', @level2type=N'COLUMN',@level2name=N'GID'
GO

EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'被控制的用户ID' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'tab_Game_Control', @level2type=N'COLUMN',@level2name=N'UserControled'
GO

EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'对游戏控制力度，值越大玩家赢的越多，值越小，玩家赢的越少' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'tab_Game_Control', @level2type=N'COLUMN',@level2name=N'ControlRate'
GO

EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'区间间隔' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'tab_Game_Control', @level2type=N'COLUMN',@level2name=N'PeriodGap'
GO

EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'控制力度，0:不设置 1:强拉回 2:弱拉回' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'tab_Game_Control', @level2type=N'COLUMN',@level2name=N'PowerControl'
GO

EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'最后更新时间' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'tab_Game_Control', @level2type=N'COLUMN',@level2name=N'UpdateTime'
GO

ALTER TABLE [dbo].[tab_Game_Control] ADD  DEFAULT ((0)) FOR [UserControled]
GO

ALTER TABLE [dbo].[tab_Game_Control] ADD  DEFAULT ((0)) FOR [ControlRate]
GO

ALTER TABLE [dbo].[tab_Game_Control] ADD  DEFAULT ((0)) FOR [PeriodGap]
GO

ALTER TABLE [dbo].[tab_Game_Control] ADD  DEFAULT ((0)) FOR [PowerControl]
GO

ALTER TABLE [dbo].[tab_Game_Control] ADD  DEFAULT (getdate()) FOR [UpdateTime]
GO

--增加一条记录
INSERT INTO [caipiaos].[dbo].[tab_Game_Control] ([UserControled], [ControlRate], [PeriodGap], [PowerControl]) VALUES ('0', '50', '100', '0');

---------------------------------------增加 tab_Game_Control_Log--------------------------------------------------------------------------------------------
USE [caipiaos]
GO

IF  EXISTS (SELECT * FROM dbo.sysobjects WHERE id = OBJECT_ID(N'[DF_tab_Game_Control_Log_ControlType]') AND type = 'D')
BEGIN
ALTER TABLE [dbo].[tab_Game_Control_Log] DROP CONSTRAINT [DF_tab_Game_Control_Log_ControlType]
END

GO

IF  EXISTS (SELECT * FROM dbo.sysobjects WHERE id = OBJECT_ID(N'[DF__tab_Game___Updat__1392CE8F]') AND type = 'D')
BEGIN
ALTER TABLE [dbo].[tab_Game_Control_Log] DROP CONSTRAINT [DF__tab_Game___Updat__1392CE8F]
END

GO

USE [caipiaos]
GO

/****** Object:  Table [dbo].[tab_Game_Control_Log]    Script Date: 09/01/2020 19:29:55 ******/
IF  EXISTS (SELECT * FROM sys.objects WHERE object_id = OBJECT_ID(N'[dbo].[tab_Game_Control_Log]') AND type in (N'U'))
DROP TABLE [dbo].[tab_Game_Control_Log]
GO

USE [caipiaos]
GO

/****** Object:  Table [dbo].[tab_Game_Control_Log]    Script Date: 09/01/2020 19:29:57 ******/
SET ANSI_NULLS ON
GO

SET QUOTED_IDENTIFIER ON
GO

SET ANSI_PADDING ON
GO

CREATE TABLE [dbo].[tab_Game_Control_Log](
	[GID] [bigint] IDENTITY(1,1) NOT NULL,
	[TypeID] [int] NOT NULL,
	[IssueNumber] [varchar](50) NOT NULL,
	[OldPremium] [varchar](50) NOT NULL,
	[OldNumber] [varchar](50) NOT NULL,
	[OldColour] [varchar](50) NOT NULL,
	[NewPremium] [varchar](50) NOT NULL,
	[NewNumber] [varchar](50) NOT NULL,
	[NewColour] [varchar](50) NOT NULL,
	[ControlType] [int] NOT NULL,
	[UpdateTime] [datetime] NOT NULL,
PRIMARY KEY CLUSTERED 
(
	[GID] ASC
)WITH (PAD_INDEX  = OFF, STATISTICS_NORECOMPUTE  = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS  = ON, ALLOW_PAGE_LOCKS  = ON) ON [PRIMARY]
) ON [PRIMARY]

GO

SET ANSI_PADDING OFF
GO

EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'自增ID' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'tab_Game_Control_Log', @level2type=N'COLUMN',@level2name=N'GID'
GO

EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'彩票种类' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'tab_Game_Control_Log', @level2type=N'COLUMN',@level2name=N'TypeID'
GO

EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'期号' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'tab_Game_Control_Log', @level2type=N'COLUMN',@level2name=N'IssueNumber'
GO

EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'更改之前随机数' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'tab_Game_Control_Log', @level2type=N'COLUMN',@level2name=N'OldPremium'
GO

EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'更改之前数字' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'tab_Game_Control_Log', @level2type=N'COLUMN',@level2name=N'OldNumber'
GO

EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'更改之前颜色' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'tab_Game_Control_Log', @level2type=N'COLUMN',@level2name=N'OldColour'
GO

EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'更改之后随机数' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'tab_Game_Control_Log', @level2type=N'COLUMN',@level2name=N'NewPremium'
GO

EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'更改之后数字' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'tab_Game_Control_Log', @level2type=N'COLUMN',@level2name=N'NewNumber'
GO

EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'更改之前颜色' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'tab_Game_Control_Log', @level2type=N'COLUMN',@level2name=N'NewColour'
GO

EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'Log类型 1:单杀,强拉,上拉 2:单杀,强拉,下拉 3:未单杀,强拉,上拉 4:未单杀,强拉,下拉 5:单杀,弱拉,上拉 6:未单杀,弱拉,上拉 7:单杀,弱拉,下拉 8:未单杀,弱拉,下拉 9:保持用户赢率为定值' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'tab_Game_Control_Log', @level2type=N'COLUMN',@level2name=N'ControlType'
GO

EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'更新时间' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'tab_Game_Control_Log', @level2type=N'COLUMN',@level2name=N'UpdateTime'
GO

ALTER TABLE [dbo].[tab_Game_Control_Log] ADD  CONSTRAINT [DF_tab_Game_Control_Log_ControlType]  DEFAULT ((0)) FOR [ControlType]
GO

ALTER TABLE [dbo].[tab_Game_Control_Log] ADD  DEFAULT (getdate()) FOR [UpdateTime]
GO


-----------------------------------------------------增加表 tab_Game_All_SelectType--------------------------------------------------------------------------------
USE [caipiaos]
GO

IF  EXISTS (SELECT * FROM dbo.sysobjects WHERE id = OBJECT_ID(N'[DF__tab_Game___Resul__0E0EFF63]') AND type = 'D')
BEGIN
ALTER TABLE [dbo].[tab_Game_All_SelectType] DROP CONSTRAINT [DF__tab_Game___Resul__0E0EFF63]
END

GO

USE [caipiaos]
GO

/****** Object:  Table [dbo].[tab_Game_All_SelectType]    Script Date: 09/01/2020 19:35:35 ******/
IF  EXISTS (SELECT * FROM sys.objects WHERE object_id = OBJECT_ID(N'[dbo].[tab_Game_All_SelectType]') AND type in (N'U'))
DROP TABLE [dbo].[tab_Game_All_SelectType]
GO

USE [caipiaos]
GO

/****** Object:  Table [dbo].[tab_Game_All_SelectType]    Script Date: 09/01/2020 19:35:38 ******/
SET ANSI_NULLS ON
GO

SET QUOTED_IDENTIFIER ON
GO

CREATE TABLE [dbo].[tab_Game_All_SelectType](
	[GID] [int] IDENTITY(1,1) NOT NULL,
	[SelectType] [nvarchar](30) NULL,
PRIMARY KEY CLUSTERED 
(
	[GID] ASC
)WITH (PAD_INDEX  = OFF, STATISTICS_NORECOMPUTE  = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS  = ON, ALLOW_PAGE_LOCKS  = ON) ON [PRIMARY]
) ON [PRIMARY]

GO

EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'自增ID' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'tab_Game_All_SelectType', @level2type=N'COLUMN',@level2name=N'GID'
GO

EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'下注类型' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'tab_Game_All_SelectType', @level2type=N'COLUMN',@level2name=N'SelectType'
GO

ALTER TABLE [dbo].[tab_Game_All_SelectType] ADD  DEFAULT (NULL) FOR [SelectType]
GO

--增加数据
INSERT INTO [caipiaos].[dbo].[tab_Game_All_SelectType] ([SelectType]) VALUES (N'0');
INSERT INTO [caipiaos].[dbo].[tab_Game_All_SelectType] ([SelectType]) VALUES (N'1');
INSERT INTO [caipiaos].[dbo].[tab_Game_All_SelectType] ([SelectType]) VALUES (N'2');
INSERT INTO [caipiaos].[dbo].[tab_Game_All_SelectType] ([SelectType]) VALUES (N'3');
INSERT INTO [caipiaos].[dbo].[tab_Game_All_SelectType] ([SelectType]) VALUES (N'4');
INSERT INTO [caipiaos].[dbo].[tab_Game_All_SelectType] ([SelectType]) VALUES (N'5');
INSERT INTO [caipiaos].[dbo].[tab_Game_All_SelectType] ([SelectType]) VALUES (N'6');
INSERT INTO [caipiaos].[dbo].[tab_Game_All_SelectType] ([SelectType]) VALUES (N'7');
INSERT INTO [caipiaos].[dbo].[tab_Game_All_SelectType] ([SelectType]) VALUES (N'8');
INSERT INTO [caipiaos].[dbo].[tab_Game_All_SelectType] ([SelectType]) VALUES (N'9');
INSERT INTO [caipiaos].[dbo].[tab_Game_All_SelectType] ([SelectType]) VALUES (N'red');
INSERT INTO [caipiaos].[dbo].[tab_Game_All_SelectType] ([SelectType]) VALUES (N'green');
INSERT INTO [caipiaos].[dbo].[tab_Game_All_SelectType] ([SelectType]) VALUES (N'violet');


-----------------------------------------------------增加表 tab_Game_Result----------------------------------------------------------------------------------------------------
USE [caipiaos]
GO

IF  EXISTS (SELECT * FROM dbo.sysobjects WHERE id = OBJECT_ID(N'[DF__tab_Game___AllTo__492FC531]') AND type = 'D')
BEGIN
ALTER TABLE [dbo].[tab_Game_Result] DROP CONSTRAINT [DF__tab_Game___AllTo__492FC531]
END

GO

IF  EXISTS (SELECT * FROM dbo.sysobjects WHERE id = OBJECT_ID(N'[DF__tab_Game___WinRa__4A23E96A]') AND type = 'D')
BEGIN
ALTER TABLE [dbo].[tab_Game_Result] DROP CONSTRAINT [DF__tab_Game___WinRa__4A23E96A]
END

GO

USE [caipiaos]
GO

/****** Object:  Table [dbo].[tab_Game_Result]    Script Date: 09/01/2020 19:40:28 ******/
IF  EXISTS (SELECT * FROM sys.objects WHERE object_id = OBJECT_ID(N'[dbo].[tab_Game_Result]') AND type in (N'U'))
DROP TABLE [dbo].[tab_Game_Result]
GO

USE [caipiaos]
GO

/****** Object:  Table [dbo].[tab_Game_Result]    Script Date: 09/01/2020 19:40:31 ******/
SET ANSI_NULLS ON
GO

SET QUOTED_IDENTIFIER ON
GO

SET ANSI_PADDING ON
GO

CREATE TABLE [dbo].[tab_Game_Result](
	[GID] [int] IDENTITY(1,1) NOT NULL,
	[TypeID] [int] NOT NULL,
	[SelectTypeNum] [varchar](20) NOT NULL,
	[SelectTypeColor] [varchar](20) NOT NULL,
	[AllTotalBonus] [bigint] NULL,
	[WinRate] [decimal](10, 2) NULL,
PRIMARY KEY CLUSTERED 
(
	[GID] ASC
)WITH (PAD_INDEX  = OFF, STATISTICS_NORECOMPUTE  = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS  = ON, ALLOW_PAGE_LOCKS  = ON) ON [PRIMARY]
) ON [PRIMARY]

GO

SET ANSI_PADDING OFF
GO

EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'自增ID' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'tab_Game_Result', @level2type=N'COLUMN',@level2name=N'GID'
GO

EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'彩票类型' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'tab_Game_Result', @level2type=N'COLUMN',@level2name=N'TypeID'
GO

EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'数字' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'tab_Game_Result', @level2type=N'COLUMN',@level2name=N'SelectTypeNum'
GO

EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'颜色' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'tab_Game_Result', @level2type=N'COLUMN',@level2name=N'SelectTypeColor'
GO

EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'派彩' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'tab_Game_Result', @level2type=N'COLUMN',@level2name=N'AllTotalBonus'
GO

EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'赢率' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'tab_Game_Result', @level2type=N'COLUMN',@level2name=N'WinRate'
GO

ALTER TABLE [dbo].[tab_Game_Result] ADD  DEFAULT ((0)) FOR [AllTotalBonus]
GO

ALTER TABLE [dbo].[tab_Game_Result] ADD  DEFAULT ((0)) FOR [WinRate]
GO
----------------------------增加数据
INSERT INTO [caipiaos].[dbo].[tab_Game_Result] ([TypeID], [SelectTypeNum], [SelectTypeColor], [AllTotalBonus], [WinRate]) VALUES ('1', '0', 'red', '0', '.00');
INSERT INTO [caipiaos].[dbo].[tab_Game_Result] ([TypeID], [SelectTypeNum], [SelectTypeColor], [AllTotalBonus], [WinRate]) VALUES ('1', '1', 'green', '0', '.00');
INSERT INTO [caipiaos].[dbo].[tab_Game_Result] ([TypeID], [SelectTypeNum], [SelectTypeColor], [AllTotalBonus], [WinRate]) VALUES ('1', '2', 'red', '0', '.00');
INSERT INTO [caipiaos].[dbo].[tab_Game_Result] ([TypeID], [SelectTypeNum], [SelectTypeColor], [AllTotalBonus], [WinRate]) VALUES ('1', '3', 'green', '0', '.00');
INSERT INTO [caipiaos].[dbo].[tab_Game_Result] ([TypeID], [SelectTypeNum], [SelectTypeColor], [AllTotalBonus], [WinRate]) VALUES ('1', '4', 'red', '0', '.00');
INSERT INTO [caipiaos].[dbo].[tab_Game_Result] ([TypeID], [SelectTypeNum], [SelectTypeColor], [AllTotalBonus], [WinRate]) VALUES ('1', '5', 'green', '0', '.00');
INSERT INTO [caipiaos].[dbo].[tab_Game_Result] ([TypeID], [SelectTypeNum], [SelectTypeColor], [AllTotalBonus], [WinRate]) VALUES ('1', '6', 'red', '0', '.00');
INSERT INTO [caipiaos].[dbo].[tab_Game_Result] ([TypeID], [SelectTypeNum], [SelectTypeColor], [AllTotalBonus], [WinRate]) VALUES ('1', '7', 'green', '0', '.00');
INSERT INTO [caipiaos].[dbo].[tab_Game_Result] ([TypeID], [SelectTypeNum], [SelectTypeColor], [AllTotalBonus], [WinRate]) VALUES ('1', '8', 'red', '0', '.00');
INSERT INTO [caipiaos].[dbo].[tab_Game_Result] ([TypeID], [SelectTypeNum], [SelectTypeColor], [AllTotalBonus], [WinRate]) VALUES ('1', '9', 'green', '0', '.00');
INSERT INTO [caipiaos].[dbo].[tab_Game_Result] ([TypeID], [SelectTypeNum], [SelectTypeColor], [AllTotalBonus], [WinRate]) VALUES ('1', '0', 'violet', '0', '.00');
INSERT INTO [caipiaos].[dbo].[tab_Game_Result] ([TypeID], [SelectTypeNum], [SelectTypeColor], [AllTotalBonus], [WinRate]) VALUES ('1', '5', 'violet', '0', '.00');
INSERT INTO [caipiaos].[dbo].[tab_Game_Result] ([TypeID], [SelectTypeNum], [SelectTypeColor], [AllTotalBonus], [WinRate]) VALUES ('2', '0', 'red', '0', '.00');
INSERT INTO [caipiaos].[dbo].[tab_Game_Result] ([TypeID], [SelectTypeNum], [SelectTypeColor], [AllTotalBonus], [WinRate]) VALUES ('2', '1', 'green', '0', '.00');
INSERT INTO [caipiaos].[dbo].[tab_Game_Result] ([TypeID], [SelectTypeNum], [SelectTypeColor], [AllTotalBonus], [WinRate]) VALUES ('2', '2', 'red', '0', '.00');
INSERT INTO [caipiaos].[dbo].[tab_Game_Result] ([TypeID], [SelectTypeNum], [SelectTypeColor], [AllTotalBonus], [WinRate]) VALUES ('2', '3', 'green', '0', '.00');
INSERT INTO [caipiaos].[dbo].[tab_Game_Result] ([TypeID], [SelectTypeNum], [SelectTypeColor], [AllTotalBonus], [WinRate]) VALUES ('2', '4', 'red', '0', '.00');
INSERT INTO [caipiaos].[dbo].[tab_Game_Result] ([TypeID], [SelectTypeNum], [SelectTypeColor], [AllTotalBonus], [WinRate]) VALUES ('2', '5', 'green', '0', '.00');
INSERT INTO [caipiaos].[dbo].[tab_Game_Result] ([TypeID], [SelectTypeNum], [SelectTypeColor], [AllTotalBonus], [WinRate]) VALUES ('2', '6', 'red', '0', '.00');
INSERT INTO [caipiaos].[dbo].[tab_Game_Result] ([TypeID], [SelectTypeNum], [SelectTypeColor], [AllTotalBonus], [WinRate]) VALUES ('2', '7', 'green', '0', '.00');
INSERT INTO [caipiaos].[dbo].[tab_Game_Result] ([TypeID], [SelectTypeNum], [SelectTypeColor], [AllTotalBonus], [WinRate]) VALUES ('2', '8', 'red', '0', '.00');
INSERT INTO [caipiaos].[dbo].[tab_Game_Result] ([TypeID], [SelectTypeNum], [SelectTypeColor], [AllTotalBonus], [WinRate]) VALUES ('2', '9', 'green', '0', '.00');
INSERT INTO [caipiaos].[dbo].[tab_Game_Result] ([TypeID], [SelectTypeNum], [SelectTypeColor], [AllTotalBonus], [WinRate]) VALUES ('2', '0', 'violet', '0', '.00');
INSERT INTO [caipiaos].[dbo].[tab_Game_Result] ([TypeID], [SelectTypeNum], [SelectTypeColor], [AllTotalBonus], [WinRate]) VALUES ('2', '5', 'violet', '0', '.00');
INSERT INTO [caipiaos].[dbo].[tab_Game_Result] ([TypeID], [SelectTypeNum], [SelectTypeColor], [AllTotalBonus], [WinRate]) VALUES ('3', '0', 'red', '0', '.00');
INSERT INTO [caipiaos].[dbo].[tab_Game_Result] ([TypeID], [SelectTypeNum], [SelectTypeColor], [AllTotalBonus], [WinRate]) VALUES ('3', '1', 'green', '0', '.00');
INSERT INTO [caipiaos].[dbo].[tab_Game_Result] ([TypeID], [SelectTypeNum], [SelectTypeColor], [AllTotalBonus], [WinRate]) VALUES ('3', '2', 'red', '0', '.00');
INSERT INTO [caipiaos].[dbo].[tab_Game_Result] ([TypeID], [SelectTypeNum], [SelectTypeColor], [AllTotalBonus], [WinRate]) VALUES ('3', '3', 'green', '0', '.00');
INSERT INTO [caipiaos].[dbo].[tab_Game_Result] ([TypeID], [SelectTypeNum], [SelectTypeColor], [AllTotalBonus], [WinRate]) VALUES ('3', '4', 'red', '0', '.00');
INSERT INTO [caipiaos].[dbo].[tab_Game_Result] ([TypeID], [SelectTypeNum], [SelectTypeColor], [AllTotalBonus], [WinRate]) VALUES ('3', '5', 'green', '0', '.00');
INSERT INTO [caipiaos].[dbo].[tab_Game_Result] ([TypeID], [SelectTypeNum], [SelectTypeColor], [AllTotalBonus], [WinRate]) VALUES ('3', '6', 'red', '0', '.00');
INSERT INTO [caipiaos].[dbo].[tab_Game_Result] ([TypeID], [SelectTypeNum], [SelectTypeColor], [AllTotalBonus], [WinRate]) VALUES ('3', '7', 'green', '0', '.00');
INSERT INTO [caipiaos].[dbo].[tab_Game_Result] ([TypeID], [SelectTypeNum], [SelectTypeColor], [AllTotalBonus], [WinRate]) VALUES ('3', '8', 'red', '0', '.00');
INSERT INTO [caipiaos].[dbo].[tab_Game_Result] ([TypeID], [SelectTypeNum], [SelectTypeColor], [AllTotalBonus], [WinRate]) VALUES ('3', '9', 'green', '0', '.00');
INSERT INTO [caipiaos].[dbo].[tab_Game_Result] ([TypeID], [SelectTypeNum], [SelectTypeColor], [AllTotalBonus], [WinRate]) VALUES ('3', '0', 'violet', '0', '.00');
INSERT INTO [caipiaos].[dbo].[tab_Game_Result] ([TypeID], [SelectTypeNum], [SelectTypeColor], [AllTotalBonus], [WinRate]) VALUES ('3', '5', 'violet', '0', '.00');
INSERT INTO [caipiaos].[dbo].[tab_Game_Result] ([TypeID], [SelectTypeNum], [SelectTypeColor], [AllTotalBonus], [WinRate]) VALUES ('4', '0', 'red', '0', '.00');
INSERT INTO [caipiaos].[dbo].[tab_Game_Result] ([TypeID], [SelectTypeNum], [SelectTypeColor], [AllTotalBonus], [WinRate]) VALUES ('4', '1', 'green', '0', '.00');
INSERT INTO [caipiaos].[dbo].[tab_Game_Result] ([TypeID], [SelectTypeNum], [SelectTypeColor], [AllTotalBonus], [WinRate]) VALUES ('4', '2', 'red', '0', '.00');
INSERT INTO [caipiaos].[dbo].[tab_Game_Result] ([TypeID], [SelectTypeNum], [SelectTypeColor], [AllTotalBonus], [WinRate]) VALUES ('4', '3', 'green', '0', '.00');
INSERT INTO [caipiaos].[dbo].[tab_Game_Result] ([TypeID], [SelectTypeNum], [SelectTypeColor], [AllTotalBonus], [WinRate]) VALUES ('4', '4', 'red', '0', '.00');
INSERT INTO [caipiaos].[dbo].[tab_Game_Result] ([TypeID], [SelectTypeNum], [SelectTypeColor], [AllTotalBonus], [WinRate]) VALUES ('4', '5', 'green', '0', '.00');
INSERT INTO [caipiaos].[dbo].[tab_Game_Result] ([TypeID], [SelectTypeNum], [SelectTypeColor], [AllTotalBonus], [WinRate]) VALUES ('4', '6', 'red', '0', '.00');
INSERT INTO [caipiaos].[dbo].[tab_Game_Result] ([TypeID], [SelectTypeNum], [SelectTypeColor], [AllTotalBonus], [WinRate]) VALUES ('4', '7', 'green', '0', '.00');
INSERT INTO [caipiaos].[dbo].[tab_Game_Result] ([TypeID], [SelectTypeNum], [SelectTypeColor], [AllTotalBonus], [WinRate]) VALUES ('4', '8', 'red', '0', '.00');
INSERT INTO [caipiaos].[dbo].[tab_Game_Result] ([TypeID], [SelectTypeNum], [SelectTypeColor], [AllTotalBonus], [WinRate]) VALUES ('4', '9', 'green', '0', '.00');
INSERT INTO [caipiaos].[dbo].[tab_Game_Result] ([TypeID], [SelectTypeNum], [SelectTypeColor], [AllTotalBonus], [WinRate]) VALUES ('4', '0', 'violet', '0', '.00');
INSERT INTO [caipiaos].[dbo].[tab_Game_Result] ([TypeID], [SelectTypeNum], [SelectTypeColor], [AllTotalBonus], [WinRate]) VALUES ('4', '5', 'violet', '0', '.00');


--------------------------------------------------------------------------------------------------------------------------



