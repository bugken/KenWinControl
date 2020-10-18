USE [9lottery]
GO

/****** Object:  StoredProcedure [dbo].[sp_GenerateGameNumberUpdateTrigger]    Script Date: 09/29/2020 17:36:34 ******/
IF  EXISTS (SELECT * FROM sys.objects WHERE object_id = OBJECT_ID(N'[dbo].[sp_GenerateGameNumberUpdateTrigger]') AND type in (N'P', N'PC'))
DROP PROCEDURE [dbo].[sp_GenerateGameNumberUpdateTrigger]
GO

USE [9lottery]
GO

/****** Object:  StoredProcedure [dbo].[sp_GenerateGameNumberUpdateTrigger]    Script Date: 09/29/2020 17:36:34 ******/
SET ANSI_NULLS ON
GO

SET QUOTED_IDENTIFIER ON
GO












CREATE PROCEDURE [dbo].[sp_GenerateGameNumberUpdateTrigger]
	@InTypeID int = 1
AS
BEGIN
	set nocount on
	
	--控制信息变量
	--declare @InTypeID int = 4
	declare @UserControled int = 0
	declare @ControlRate int = 0
	declare @PeriodGap int = 0
	declare @PowerControl int = 0
	declare @Enabled int = 0
	declare @TypeIDFromTable int = 0
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
	declare @IssueStartTime datetime = getdate() --游戏开始时间
	declare @MinutesElapse int = datediff(minute, convert(datetime,convert(varchar(10),getdate(),120)), @CurrentTimeNextMin)--距离凌晨的分钟数
	
	--select * from [9lottery].[dbo].tab_Games where State=0 and StartTime<=@CurrentTime
	select top 5 * into #tabTmpGame from [9lottery].[dbo].tab_Games where State=0 and StartTime<=@CurrentTime and TypeID = @InTypeID order by StartTime desc
	select @Counts = count(*) from #tabTmpGame
	print '当前期数@Counts:' + cast(@Counts as varchar(30))
	while @LoopCounts < @Counts 
	begin
		declare @StartTime datetime = getdate()
		print '-----------------------------------begin------------------------------------------------'
		set @LoopCounts = @LoopCounts + 1
		
		--获取开奖期号
		select @CurrentIssueNumber = IssueNumber, @IntervalM = IntervalM, @OptState = OptState,@IssueStartTime = StartTime, @TypeIDFromTable = TypeID from  
				(select row_number() over(order by StartTime desc, TypeID asc) as rowid, * from #tabTmpGame) as t 
			where rowid=@LoopCounts 
		if @MinutesElapse = 0--考虑凌晨情况
			set @MinutesElapse = 1
		if @MinutesElapse%@IntervalM <> 0
		begin
			print '没有开奖'
			continue
		end
		if @OptState=1
		begin
			print '已经预设'
			continue
		end
		--判断游戏开始时间，避免之前游戏开奖后State没有置1
		if datediff(minute, @IssueStartTime, @CurrentTime) >= @IntervalM
		begin
			print '上期开奖没有置1,期号@CurrentIssueNumber' + @CurrentIssueNumber
			continue
		end
		
		--获取控制信息
		select top 1 @Enabled = ControlEnabled, @UserControled = ControledUserID, @ControlRate = isnull(ControlRate, 30), @PeriodGap = isnull(ControlPeriodGap, 100), 
			@PowerControl = ControlPower from [9lottery].[dbo].tab_GameType where TypeID = @TypeIDFromTable order by TypeID
		if @PowerControl > 2
			set @PowerControl = 2
		else if @PowerControl < 0
			set @PowerControl = 0
		if @ControlRate <= 0
			set @ControlRate = 30
		print '受控用户@UserControled:' + cast(@UserControled as varchar(10))
		print '控制指数@ControlRate:' + cast(@ControlRate as varchar(10))
		print '选取期数区间@PeriodGap:' + cast(@PeriodGap as varchar(10))
		print '强弱控制@PowerControl:' + cast(@PowerControl as varchar(10))
		print '是否是能@Enabled:' + cast(@Enabled as varchar(10))
		
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
		print '当前期数@CurrentIssueNumber:' + @CurrentIssueNumber
		print '上一期数@LastIssueNumber:' + @LastIssueNumber
		print '起始期数@BeginIssueNumber:' + @BeginIssueNumber 
		print 'Game类型@TypeID:' + cast(@TypeID as varchar(2)) 
		print '是否预设@OptState:' + cast(@OptState as varchar(10)) 
		
		--调用存储过程处理开奖
		if @Enabled = 1 --判断单个游戏是否开启
		begin
			print '开始游戏控制@TypeID:' + cast(@TypeID as varchar(10))
			execute sp_GenerateGameNumberUpdate @UserControled, @ControlRate, @PowerControl, 
						@TypeID, @OptState, @BeginIssueNumber, @CurrentIssueNumber, @LastIssueNumber
		end
		
		declare @EndTime datetime = getdate()
		declare @Miliseconds int = datediff(ms, @StartTime, @EndTime)
		print '------------------------end time eclaps:' + cast(@Miliseconds as varchar(20)) + '-------------------------------------'
	end
	drop table #tabTmpGame
END








GO


