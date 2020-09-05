USE [9lottery]
GO

/****** Object:  StoredProcedure [dbo].[sp_GenerateGameNumberUpdateTrigger]    Script Date: 09/04/2020 20:22:17 ******/
IF  EXISTS (SELECT * FROM sys.objects WHERE object_id = OBJECT_ID(N'[dbo].[sp_GenerateGameNumberUpdateTrigger]') AND type in (N'P', N'PC'))
DROP PROCEDURE [dbo].[sp_GenerateGameNumberUpdateTrigger]
GO

USE [9lottery]
GO

/****** Object:  StoredProcedure [dbo].[sp_GenerateGameNumberUpdateTrigger]    Script Date: 09/04/2020 20:22:17 ******/
SET ANSI_NULLS ON
GO

SET QUOTED_IDENTIFIER ON
GO








CREATE PROCEDURE [dbo].[sp_GenerateGameNumberUpdateTrigger]
AS
BEGIN
	--控制开关未开启或者已经预设,不进行控制
	if (select ISNULL(GameUpdateNumberOpen,0) from [9lottery].dbo.tab_GameNumberSet) = 0
	begin
		--print '控制开关未开启或该期已经预设'
		return
	end
	--获取控制信息
	declare @UserControled int = 0
	declare @ControlRate int = 0
	declare @PeriodGap int = 0
	declare @PowerControl int = 0
	select top 1 @UserControled = UserControled, @ControlRate = isnull(ControlRate, 30), @PeriodGap = isnull(PeriodGap, 100), @PowerControl = PowerControl
		from [9lottery].[dbo].tab_Game_Control order by UpdateTime desc
	if @PowerControl > 2
		set @PowerControl = 2
	else if @PowerControl < 0
		set @PowerControl = 0
	if @ControlRate <= 0
		set @ControlRate = 30
	--print '受控用户@UserControled:' + cast(@UserControled as varchar(10))
	--print '控制指数@ControlRate:' + cast(@ControlRate as varchar(10))
	--print '选取期数区间@PeriodGap:' + cast(@PeriodGap as varchar(10))
	--print '强弱控制@PowerControl:' + cast(@PowerControl as varchar(10))
	
	--循环处理开奖
	declare @CurrentTime datetime = getdate()
	declare @CurrentTimeNextMin datetime = dateadd(minute,1,getdate())
	declare @Counts int = 0
	declare @LoopCounts int = 0
	declare @OptState int = -1
	declare @IntervalM int = 0
	declare @CurrentIssueNumber varchar(60) = ''
	declare @BeginIssueNumber varchar(30) = ''
	declare @LastIssueNumber varchar(30) = ''
	declare @MinutesElapse int = datediff(minute, convert(datetime,convert(varchar(10),getdate(),120)), @CurrentTimeNextMin)--距离凌晨的分钟数
	select @Counts = count(*) from [9lottery].[dbo].tab_Games where State=0 and StartTime<=@CurrentTime
	while @LoopCounts < @Counts and @LoopCounts < 4
	begin
		print '-----------------------------------begin------------------------------------------------'
		--获取开奖期号
		set @LoopCounts = @LoopCounts + 1
		select @CurrentIssueNumber = IssueNumber, @IntervalM = IntervalM from  
				(select row_number() over(order by StartTime desc, TypeID asc) as rowid, * from [9lottery].[dbo].tab_Games 
					where State=0 and StartTime<=@CurrentTime) as t 
			where rowid=@LoopCounts 
		if @MinutesElapse = 0--考虑凌晨情况
			set @MinutesElapse = 1
		if @MinutesElapse%@IntervalM <> 0
			continue
		--计算@LastIssueNumber @BeginIssueNumber
		set @LastIssueNumber = cast((cast(@CurrentIssueNumber as bigint)-1) as varchar(30))
		declare @VarDay int = cast(substring(@CurrentIssueNumber, 0, 9) as int)
		declare @TypeID int = cast(substring(@CurrentIssueNumber, 9, 1) as int)
		declare @Period int = cast(substring(@CurrentIssueNumber, 10, 4) as int)
		if @PeriodGap <= @Period
		begin 
			declare @TmpNumber bigint = cast(@CurrentIssueNumber as bigint) - @PeriodGap
			set @BeginIssueNumber = cast(@TmpNumber as varchar(30))
		end
		else if @PeriodGap > @Period
		begin 
			declare @BaseNum int = 24*60/@IntervalM
			declare @PeriodNotInToday int = @PeriodGap - @Period
			declare @DayNums int = 0 - (@PeriodNotInToday / @BaseNum + 1)
			declare @PeriodNums int = @BaseNum - @PeriodNotInToday % @BaseNum
			declare @TargetDate datetime = dateadd(Day, @DayNums, CONVERT(varchar(12), getdate(), 112))
			declare @VarTargetDate varchar(30) = CONVERT(varchar(12), @TargetDate, 112)
			if len(@PeriodNums) = 1
				set @BeginIssueNumber = @VarTargetDate + cast(@TypeID as varchar(2)) + '000' + cast(@PeriodNums as varchar(10))
			else if len(@PeriodNums) = 2
				set @BeginIssueNumber = @VarTargetDate + cast(@TypeID as varchar(2)) +'00' + cast(@PeriodNums as varchar(10))
			else if len(@PeriodNums) = 3
				set @BeginIssueNumber = @VarTargetDate + cast(@TypeID as varchar(2)) +'0' + cast(@PeriodNums as varchar(10))
			else
				set @BeginIssueNumber = @VarTargetDate + cast(@TypeID as varchar(2)) +cast(@PeriodNums as varchar(10))
		end
		--print '当前期数@CurrentIssueNumber:' + @CurrentIssueNumber
		--print '上一期数@LastIssueNumber:' + @LastIssueNumber
		--print '起始期数@BeginIssueNumber:' + @BeginIssueNumber 
		--print 'Game类型@TypeID:' + cast(@TypeID as varchar(2)) 
		--print '是否预设@OptState:' + cast(@OptState as varchar(10)) 

		--调用存储过程处理开奖
		execute sp_GenerateGameNumberUpdate @UserControled, @ControlRate, @PowerControl, 
					@TypeID, @OptState, @BeginIssueNumber, @CurrentIssueNumber, @LastIssueNumber
		
		print '-----------------------------------end------------------------------------------------'
	end
END














GO


