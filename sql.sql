USE [caipiaos]
GO

/****** Object:  StoredProcedure [dbo].[sp_Get_CurrentStatatics]    Script Date: 08/28/2020 17:38:12 ******/
IF  EXISTS (SELECT * FROM sys.objects WHERE object_id = OBJECT_ID(N'[dbo].[sp_Get_CurrentStatatics]') AND type in (N'P', N'PC'))
DROP PROCEDURE [dbo].[sp_Get_CurrentStatatics]
GO

USE [caipiaos]
GO

/****** Object:  StoredProcedure [dbo].[sp_Get_CurrentStatatics]    Script Date: 08/28/2020 17:38:12 ******/
SET ANSI_NULLS ON
GO

SET QUOTED_IDENTIFIER ON
GO

CREATE PROCEDURE [dbo].[sp_Get_CurrentStatatics]
AS
BEGIN
	declare @UserControled int = 0
	declare @ControlRate int = 0
	declare @PeriodGap int = 0
	select top 1 @UserControled = UserControled, @ControlRate = ControlRate, @PeriodGap = PeriodGap from caipiaos.dbo.tab_Game_Control order by UpdateTime desc
	print '选取期数区间@PeriodGap:' + cast(@PeriodGap as varchar(10))
	print '控制指数@ControlRate:' + cast(@ControlRate as varchar(10))
	--单杀情况 单杀的信息需要写入到数据库
	
	--计算区间投注金额 派彩金额,区间没有设置，默认当天作为区间
	if (select ISNULL(GameUpdateNumberOpen,0) from caipiaos.dbo.tab_GameNumberSet)=1 and @ControlRate <> 0
	begin
		declare @BonusAlready decimal(20, 2) = 0.0 --派彩金额
		declare @AllBet decimal(20, 2) = 0.0 --投注金额
		declare @CurrentIssueNumber varchar(30) = ''
		declare @StartIssueNumber varchar(30) = ''
		declare @CurrentTime datetime = GETDATE()
		declare @LastPeriodTime datetime = DATEADD(mi,-3,GETDATE()) --上一期时间
		declare @DateTodayZero datetime = DATEADD(DAY,0,DATEDIFF(DAY,0,GETDATE()))
		declare @DateTomorrow datetime = DATEADD(DAY,1,DATEDIFF(DAY,0,GETDATE()))
		select top 1 @CurrentIssueNumber = IssueNumber from caipiaos.dbo.tab_Games where OptState!=1 and State=0 and StartTime<=@CurrentTime;
		if @CurrentIssueNumber = '' or @CurrentIssueNumber is null
			return --已经预设
		print '当前(终止)期数@CurrentIssueNumber:' + @CurrentIssueNumber 

		if @PeriodGap is not null and @PeriodGap <> 0 
		begin
			--对@PeriodGap进行判断处理
			declare @VarDay int = cast(substring(@CurrentIssueNumber, 0, 9) as int)
			declare @Period int = cast(substring(@CurrentIssueNumber, 9, 3) as int)
			if @PeriodGap < @Period
			begin 
				declare @TmpNumber bigint = cast(@CurrentIssueNumber as bigint) - @PeriodGap
				set @StartIssueNumber = cast(@TmpNumber as varchar(30))
			end
			else if @PeriodGap > @Period
			begin 
				declare @PeriodNotInToday int = @PeriodGap - @Period
				declare @DayNums int = 0 - (@PeriodNotInToday / 480 + 1)
				declare @PeriodNums int = 480 - @PeriodNotInToday % 480
				declare @TargetDate datetime = dateadd(Day, @DayNums, CONVERT(varchar(12), getdate(), 112))
				declare @VarTargetDate varchar(30) = CONVERT(varchar(12), @TargetDate, 112)
				if len(@PeriodNums) = 1
					set @StartIssueNumber = @VarTargetDate + '00' + cast(@PeriodNums as varchar(4))
				else if len(@PeriodNums) = 2
					set @StartIssueNumber = @VarTargetDate + '0' + cast(@PeriodNums as varchar(4))
				else
					set @StartIssueNumber = @VarTargetDate + cast(@PeriodNums as varchar(4))
			end
			print '起始期数@StartIssueNumber:' + @StartIssueNumber 
			select @AllBet = sum(RealAmount) from caipiaos.dbo.tab_GameOrder where IssueNumber >= @StartIssueNumber and IssueNumber <= @CurrentIssueNumber
			select @BonusAlready = sum(ProfitAmount - RealAmount) from caipiaos.dbo.tab_GameOrder where IssueNumber >= @StartIssueNumber and IssueNumber < @CurrentIssueNumber
		end
		else
		begin 
			--默认按照一天计算
			print '默认区间为当天'
			select @AllBet = sum(RealAmount) from caipiaos.dbo.tab_GameOrder where SettlementTime between @DateTodayZero and @CurrentTime
			select @BonusAlready = sum(ProfitAmount - RealAmount) from caipiaos.dbo.tab_GameOrder where SettlementTime between @DateTodayZero and @LastPeriodTime
		end
		print '投注金额@AllBet:' + isnull(cast(@AllBet as varchar(20)),0)
		print '已派彩金额@BonusAlready:' + isnull(cast(@BonusAlready as varchar(20)),0)
		
		--计算每种结果的输赢金额
		declare @LotteryGame varchar(50) = '0,1,2,3,4,5,6,7,8,9,red,green,violet'
		--select  CHARINDEX('violet', @LotteryGame)/2
		declare @Result varchar(10) = ''
		declare @Index int = 0
		declare @MultiRate decimal(2, 1) = 0.0
		declare @TotalBonus int = 0
		--#LotteryEveryBonus记录每注输赢的临时表
		create table #LotteryTotalBonus(TypeID int, IssueNumber varchar(30), SelectType varchar(20), TotalBonus bigint)
		declare CursorResult cursor for select SelectType from caipiaos.dbo.tab_Game_All_SelectType ORDER BY SelectType
		open CursorResult
		fetch next from CursorResult into @Result
		while @@FETCH_STATUS = 0
		begin
			select @Index = CHARINDEX(@Result, @LotteryGame)/2
			if @Index < 10 --数字
				set @MultiRate = 8
			else if @Index = 10	or @Index = 12--red green
				set @MultiRate = 1
			else if @Index = 15 --violet
				set @MultiRate = 4.5
			insert into #LotteryTotalBonus(TypeID, IssueNumber, SelectType, TotalBonus) 
				select TypeID, IssueNumber, SelectType, sum(RealAmount) * @MultiRate TotalBonus 
					from caipiaos.dbo.tab_GameOrder where @Result = SelectType and @CurrentIssueNumber = IssueNumber group by IssueNumber, TypeID, SelectType

			fetch next from CursorResult into @Result
		end				
		close CursorResult
		deallocate CursorResult
		
		--玩家没有下注，直接返回
		declare @BetCounts int = 0
		select @BetCounts = count(*) from #LotteryTotalBonus
		if @BetCounts = 0
		begin
			print '玩家没有下注'
			drop table #LotteryTotalBonus
			return
		end
		select TypeID, SelectType, IssueNumber, TotalBonus from #LotteryTotalBonus
		--计算所有色彩派彩
		declare @RedBonus decimal(10, 2) = 0.0
		declare @GreenBonus decimal(10, 2) = 0.0
		declare @VioletBonus decimal(10, 2) = 0.0
		select @RedBonus = sum(TotalBonus) from #LotteryTotalBonus where SelectType = 'red'
		select @GreenBonus = sum(TotalBonus) from #LotteryTotalBonus where SelectType = 'green'
		select @VioletBonus = sum(TotalBonus)/2 from #LotteryTotalBonus where SelectType = 'violet'
		--添加彩票所有可能的结果
		create table #LotteryResult(TypeID int, IssueNumber varchar(50), SelectTypeNum varchar(20), SelectTypeColor varchar(20), AllTotalBonus bigint, WinRate decimal(10, 3))
		insert into #LotteryResult(TypeID, IssueNumber, SelectTypeNum, SelectTypeColor, AllTotalBonus, WinRate) 
			select TypeID, @CurrentIssueNumber, SelectTypeNum, SelectTypeColor, AllTotalBonus, 0.0 from caipiaos.dbo.tab_Game_Result
		--算出10种结果对应的输赢
		/*
		insert into #LotteryResult(TypeID, IssueNumber, SelectTypeNum, SelectTypeColor, AllTotalBonus, WinRate) 
			select a.TypeID, a.IssueNumber, a.SelectType, 'red', sum(a.TotalBonus) + sum(isnull(b.TotalBonus,0)), 0.0 from #LotteryTotalBonus a
				left join #LotteryTotalBonus b on a.TypeID = b.TypeID and a.SelectType in('2','4','6','8') and b.SelectType = 'red'
					where a.SelectType in('2','4','6','8') group by a.IssueNumber, a.TypeID, a.SelectType
		insert into #LotteryResult(TypeID, IssueNumber, SelectTypeNum, SelectTypeColor, AllTotalBonus, WinRate) 
			select a.TypeID, a.IssueNumber, a.SelectType, 'green', sum(a.TotalBonus) + sum(isnull(b.TotalBonus,0)), 0.0 from #LotteryTotalBonus a
				left join #LotteryTotalBonus b on a.TypeID = b.TypeID and a.SelectType in('1','3','7','9') and b.SelectType = 'green'
					where a.SelectType in('1','3','7','9') group by a.IssueNumber, a.TypeID, a.SelectType					
		insert into #LotteryResult(TypeID, IssueNumber, SelectTypeNum, SelectTypeColor, AllTotalBonus, WinRate) 
			select a.TypeID, a.IssueNumber, a.SelectType, case when a.SelectType = '0' then 'red,violet' when a.SelectType = '5' then 'green,violet' end, 
				sum(a.TotalBonus) + sum(isnull(b.TotalBonus,0))/2, 0.0 from #LotteryTotalBonus a
				left join #LotteryTotalBonus b on a.TypeID = b.TypeID and a.SelectType in('0','5') and b.SelectType = 'violet'
					where a.SelectType in('0','5') group by a.IssueNumber, a.TypeID, a.SelectType
		select * from #LotteryResult order by TypeID
		*/
		UPDATE #LotteryResult SET #LotteryResult.AllTotalBonus = t1.AllTotalBonus + t2.TotalBonus FROM #LotteryResult t1
		INNER JOIN #LotteryTotalBonus t2 ON t1.TypeID = t2.TypeID and (t1.SelectTypeNum = t2.SelectType or t1.SelectTypeColor = t2.SelectType)
		select * from #LotteryResult order by TypeID, WinRate desc
		--整合重复的结果
		create table #LotteryResultFinal(TypeID int, IssueNumber varchar(50), SelectTypeNum varchar(20), SelectTypeColor varchar(20), AllTotalBonus bigint, WinRate decimal(10, 3))
		insert into #LotteryResultFinal(TypeID, IssueNumber, SelectTypeNum, SelectTypeColor, AllTotalBonus, WinRate) 
			select TypeID, IssueNumber, SelectTypeNum, SelectTypeColor, sum(AllTotalBonus), 0.0 from #LotteryResult 
				group by TypeID, IssueNumber, SelectTypeNum, SelectTypeColor
		drop table #LotteryResult
		
		--处理单杀情况，单杀只杀某ID最大中奖的下注，将玩家下注从表#LotteryResultFinal中去掉即可，档位控制的话，需要去掉一个档位
		
		--计算WinRate
		declare @TargetControlRate decimal(4,2) = (@ControlRate+0.0)/100
		print '目标赢率@TargetControlRate:' + cast(@TargetControlRate as varchar(20))
		update #LotteryResultFinal set WinRate = (@BonusAlready+AllTotalBonus)/@AllBet
		select * from #LotteryResultFinal order by TypeID, WinRate desc
		
		--更新游戏表并写入日志
		declare @NumBegin Int=1000    --随机数的最小值 
		declare @NumEnd Int=9999   --随机数的最大值 
		declare @RandNum int = 0
		declare @Loops int = 0
		declare @IssueNumber varchar(50) = ''
		declare @SelectTypeNum varchar(20) = ''
		declare @SelectTypeColor varchar(20) = ''
		declare @WinRate decimal(10, 2) = 0.0
		declare @BeforeSelectTypeNum varchar(20) = ''
		declare @BeforeSelectTypeColor varchar(20) = ''
		declare @BeforePrenium varchar(20) = ''
		declare @TypeNum int = 0
		declare @VarTypeID int = 0
		
		declare CursorTypeID cursor for select TypeID from caipiaos.dbo.tab_GameType
		open CursorTypeID
		fetch next from CursorTypeID into @VarTypeID
		while @@FETCH_STATUS = 0
		begin
			--玩家没有下注不控制
			select @BetCounts = count(*) from #LotteryTotalBonus where TypeID = @VarTypeID
			if @BetCounts = 0
			begin
				print '彩票类型' + cast(@VarTypeID as varchar(10)) + '没有玩家下注' 
				fetch next from CursorTypeID into @VarTypeID
				continue
			end

			set @Loops = 0
			set @RandNum = @NumBegin+(@NumEnd-@NumBegin)*rand()
			set @RandNum =  @RandNum * 10
			select @BeforePrenium = Premium, @BeforeSelectTypeNum = Number, @BeforeSelectTypeColor = Colour from caipiaos.dbo.tab_Games where TypeID = @VarTypeID and IssueNumber = @CurrentIssueNumber
			print '更改前随机数:' + @BeforePrenium + ',更改前中奖数字:' + @BeforeSelectTypeNum + ',更改前中奖颜色:' + @BeforeSelectTypeColor
			declare CursorUpdate cursor for select IssueNumber, SelectTypeNum, SelectTypeColor, WinRate 
					from #LotteryResultFinal where TypeID = @VarTypeID ORDER BY WinRate desc
			open CursorUpdate
			fetch next from CursorUpdate into @IssueNumber, @SelectTypeNum, @SelectTypeColor, @WinRate
			while @@FETCH_STATUS = 0
			begin
				set @Loops = @Loops + 1
				if @Loops = 10
				begin
					set @TypeNum = cast(@SelectTypeNum as int)
					set @RandNum = @RandNum + @TypeNum
					update caipiaos.dbo.tab_Games set Premium = @RandNum, Number = @SelectTypeNum, Colour = @SelectTypeColor
						where TypeID = @VarTypeID and IssueNumber = @IssueNumber
					insert into caipiaos.dbo.tab_Game_Control_Log(TypeID, IssueNumber, OldPremium, OldNumber, OldColour, NewPremium, NewNumber, NewColour, ControlType, UpdateTime)
						values(@VarTypeID, @IssueNumber, @BeforePrenium, @BeforeSelectTypeNum, @BeforeSelectTypeColor, @RandNum, @SelectTypeNum, @SelectTypeColor, 1, getdate())
					break
				end
				if @WinRate <= @TargetControlRate
				begin
					set @TypeNum = cast(@SelectTypeNum as int)
					set @RandNum = @RandNum + @TypeNum
					update caipiaos.dbo.tab_Games set Premium = @RandNum, Number = @SelectTypeNum, Colour = @SelectTypeColor
						where TypeID = @VarTypeID and IssueNumber = @IssueNumber
					insert into caipiaos.dbo.tab_Game_Control_Log(TypeID, IssueNumber, OldPremium, OldNumber, OldColour, NewPremium, NewNumber, NewColour, ControlType, UpdateTime)
						values(@VarTypeID, @IssueNumber, @BeforePrenium, @BeforeSelectTypeNum, @BeforeSelectTypeColor, @RandNum, @SelectTypeNum, @SelectTypeColor, 1, getdate())
					break 
				end
				fetch next from CursorUpdate into @IssueNumber, @SelectTypeNum, @SelectTypeColor, @WinRate
			end				
			close CursorUpdate
			deallocate CursorUpdate
			
			fetch next from CursorTypeID into @VarTypeID
		end				
		close CursorTypeID
		deallocate CursorTypeID
		
		drop table #LotteryTotalBonus
		drop table #LotteryResultFinal
	end
END
GO


