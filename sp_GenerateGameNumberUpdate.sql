USE [9lottery]
GO

/****** Object:  StoredProcedure [dbo].[sp_GenerateGameNumberUpdate]    Script Date: 09/18/2020 19:26:36 ******/
IF  EXISTS (SELECT * FROM sys.objects WHERE object_id = OBJECT_ID(N'[dbo].[sp_GenerateGameNumberUpdate]') AND type in (N'P', N'PC'))
DROP PROCEDURE [dbo].[sp_GenerateGameNumberUpdate]
GO

USE [9lottery]
GO

/****** Object:  StoredProcedure [dbo].[sp_GenerateGameNumberUpdate]    Script Date: 09/18/2020 19:26:36 ******/
SET ANSI_NULLS ON
GO

SET QUOTED_IDENTIFIER ON
GO









CREATE PROCEDURE [dbo].[sp_GenerateGameNumberUpdate]
	@InUserControled int = 0,
	@InControlRate int = 30,
	@InPowerControl int = 0,
	@InTypeID int = 1,
	@InOptState int = 1,
	@InBeginIssueNumber varchar(30) = '',
	@InCurrentIssueNumber varchar(30) = '',
	@InLastIssueNumber varchar(30) = ''
AS
BEGIN
	--控制开关未开启或者已经预设,不进行控制
	if (select ISNULL(GameUpdateNumberOpen,0) from [9lottery].dbo.tab_GameNumberSet) = 0 or @InOptState = 1
	begin
		print '控制开关未开启或该期已经预设'
		return
	end
	
	--投注人数不得少于5，否则不调控
	declare @UserCounts int = 0
	select @UserCounts = count(UserCounts) from 
			(select count(distinct UserID) UserCounts from [9lottery].[dbo].tab_GameOrder where IssueNumber = @InCurrentIssueNumber and TypeID = @InTypeID group by UserID) as t
	if @UserCounts <= 5
	begin
		--print '下注人数小于5个人'
		--return
		print '' 
	end
	
	--计算区间投注金额 派彩金额
	declare @BonusAlready decimal(20, 2) = 0.0 --派彩金额
	declare @AllBet decimal(20, 2) = 0.0 --投注金额
	declare @AllBetUntilLast decimal(20, 2) = 0.0 --截止上期投注金额
	declare @WinRateAsOfLast decimal(20, 2) = 0.0 --截止上期玩家赢率
	select @AllBet = sum(RealAmount) from [9lottery].dbo.tab_GameOrder where TypeID = @InTypeID and IssueNumber >= @InBeginIssueNumber and IssueNumber <= @InCurrentIssueNumber
	select @AllBetUntilLast = sum(RealAmount) from [9lottery].dbo.tab_GameOrder where TypeID = @InTypeID and IssueNumber >= @InBeginIssueNumber and IssueNumber <= @InLastIssueNumber
	select @BonusAlready = sum(ProfitAmount - RealAmount) from [9lottery].dbo.tab_GameOrder where TypeID = @InTypeID and IssueNumber >= @InBeginIssueNumber and IssueNumber <= @InLastIssueNumber
	set @WinRateAsOfLast = isnull(@BonusAlready / @AllBetUntilLast, 0)
	print '投注金额@AllBet:' + isnull(cast(@AllBet as varchar(20)),0)
	print '截止上期投注金额@AllBetUntilLast:' + isnull(cast(@AllBetUntilLast as varchar(20)),0)
	print '已派彩金额@BonusAlready:' + isnull(cast(@BonusAlready as varchar(20)),0)
	print '截止上期赢率@WinRateAsOfLast:' + isnull(cast(@WinRateAsOfLast as varchar(20)),0)
	
	--计算每种结果的输赢金额
	declare @GameAllType varchar(50) = '0,1,2,3,4,5,6,7,8,9,violet,red,green,big,small'
	declare @Result varchar(10) = ''
	declare @Index int = 0
	declare @MultiRate decimal(2, 1) = 1.0
	declare @TotalBonus int = 0
	--#LotteryTotalBonus记录输赢的临时表
	create table #LotteryTotalBonus(TypeID int, IssueNumber varchar(30), SelectType varchar(20), TotalBonus bigint)
	create table #UserControledBonus(TypeID int, IssueNumber varchar(30), SelectType varchar(20), TotalBonus bigint)
	declare CursorResult cursor for select SelectType from [9lottery].dbo.tab_Game_All_SelectType ORDER BY SelectType
	open CursorResult
	fetch next from CursorResult into @Result
	while @@FETCH_STATUS = 0
	begin
		select @Index = charindex(@Result, @GameAllType)
		if @Index < 20 --数字
			set @MultiRate = 8
		else if @Index = 21 --violet
			set @MultiRate = 4.5
		else if @Index >= 28 --red:28 green:32 big:38 small:42
			set @MultiRate = 1
		insert into #LotteryTotalBonus(TypeID, IssueNumber, SelectType, TotalBonus) 
			select TypeID, IssueNumber, SelectType, sum(RealAmount) * @MultiRate TotalBonus 
				from [9lottery].dbo.tab_GameOrder where TypeID = @InTypeID and @Result = SelectType and @InCurrentIssueNumber = IssueNumber 
					group by IssueNumber, TypeID, SelectType
		--单人下注信息计算
		insert into #UserControledBonus(TypeID, IssueNumber, SelectType, TotalBonus) 
			select TypeID, IssueNumber, SelectType, sum(RealAmount) * @MultiRate TotalBonus 
				from [9lottery].dbo.tab_GameOrder where TypeID = @InTypeID and @Result = SelectType and @InCurrentIssueNumber = IssueNumber and UserID = @InUserControled
					group by IssueNumber, TypeID, SelectType

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
		drop table #UserControledBonus
		return
	end
	select TypeID, SelectType, IssueNumber, TotalBonus from #LotteryTotalBonus
	
	--获取彩票所有可能出现的结果
	create table #LotteryResult(TypeID int, IssueNumber varchar(50), SelectTypeNum varchar(20), SelectTypeColor varchar(20), AllTotalBonus bigint, WinRate decimal(10, 3))
	insert into #LotteryResult(TypeID, IssueNumber, SelectTypeNum, SelectTypeColor, AllTotalBonus, WinRate) 
		select TypeID, @InCurrentIssueNumber, SelectTypeNum, SelectTypeColor, AllTotalBonus, 0.0 from [9lottery].dbo.tab_Game_Result where TypeID = @InTypeID
	--算出11种结果对应的派彩(10 violet)
	update #LotteryResult set #LotteryResult.AllTotalBonus += isnull(t2.TotalBonus,0) from #LotteryResult t1
		inner join #LotteryTotalBonus t2 on t1.TypeID = t2.TypeID and (t1.SelectTypeNum = t2.SelectType or t2.SelectType = t1.SelectTypeColor)
	--加入大小下注的派彩
	update #LotteryResult set #LotteryResult.AllTotalBonus += isnull(t2.TotalBonus,0) from #LotteryResult t1  
		inner join #LotteryTotalBonus t2 on t1.TypeID = t2.TypeID and t2.SelectType='big' and t1.SelectTypeNum in ('5','6','7','8','9')
	update #LotteryResult set #LotteryResult.AllTotalBonus += isnull(t2.TotalBonus,0) from #LotteryResult t1  
		inner join #LotteryTotalBonus t2 on t1.TypeID = t2.TypeID and t2.SelectType='small' and t1.SelectTypeNum in ('0','1','2','3','4')
	--加上violet下注的派彩并删除violet记录
	update #LotteryResult set #LotteryResult.AllTotalBonus += isnull(t2.TotalBonus,0) from #LotteryResult t1  
		inner join #LotteryTotalBonus t2 on t1.TypeID = t2.TypeID and t2.SelectType='violet' and t1.SelectTypeNum in ('0','5')
	delete from #LotteryResult where SelectTypeColor = 'violet' and SelectTypeNum = '10'

	--更改结果的颜色
	update #LotteryResult SET SelectTypeColor = (case SelectTypeNum when '0' then 'red,violet' when '5' then 'green,violet' else SelectTypeColor end)
	select * from #LotteryResult order by TypeID, AllTotalBonus desc
	--合并出10种结果
	create table #LotteryResultFinal(TypeID int, IssueNumber varchar(50), SelectTypeNum varchar(20), SelectTypeColor varchar(20), AllTotalBonus bigint, WinRate decimal(10, 3))
	insert into #LotteryResultFinal(TypeID, IssueNumber, SelectTypeNum, SelectTypeColor, AllTotalBonus, WinRate) 
		select TypeID, IssueNumber, SelectTypeNum, SelectTypeColor, sum(AllTotalBonus), 0.0 from #LotteryResult 
			group by TypeID, IssueNumber, SelectTypeNum, SelectTypeColor
	drop table #LotteryResult
	
	--计算WinRate
	declare @TargetControlRate decimal(4,2) = (@InControlRate+0.0)/100
	print '目标赢率@TargetControlRate:' + cast(@TargetControlRate as varchar(20))
	update #LotteryResultFinal set WinRate = (isnull(@BonusAlready, 0)+AllTotalBonus)/@AllBet
	--select TypeID, SelectType, IssueNumber, TotalBonus from #UserControledBonus
	select * from #LotteryResultFinal order by TypeID, WinRate desc
	
	--更新游戏表并写入日志
	declare @RandNumVar varchar(20) = ''
	declare @Loops int = 0
	declare @IssueNumber varchar(50) = ''
	declare @WinRate decimal(10, 3) = 0.0
	declare @SelectTypeNum varchar(20) = ''
	declare @SelectTypeColor varchar(20) = ''
	declare @FinalTypeNum varchar(20) = ''
	declare @FinalTypeColor varchar(20) = ''
	declare @LogTypeNum varchar(20) = ''--用于记录Log
	declare @LogTypeColor varchar(20) = ''--用于记录Log
	declare @BeforeSelectTypeNum varchar(20) = ''
	declare @BeforeSelectTypeColor varchar(20) = ''
	declare @BeforePrenium varchar(20) = ''
	declare @TypeNum int = 0
	declare @PushUp bit = 0 --是否将用户赢率上拉向目标赢率靠近
	declare @BingoCounts int = 0
	declare @FirstLowWinRatePos int = 0 --第一个小值的位置
	declare @IsFound bit = 0 --是否继续找小值位置
	declare @StopPos int = 10 --强弱拉遍历停止位置
	declare @StepCounts int = 10 --遍历总数
	declare @IsUserControl bit = 0 --是否单控
	declare @UserControlType varchar(10) = '' --受控的类型
	declare @LogControlType int = 0 --Log类型 1:单杀,强拉,上拉 2:单杀,强拉,下拉 3:未单杀,强拉,上拉 4:未单杀,强拉,下拉 5:单杀,弱拉,上拉
									--6:未单杀,弱拉,上拉 7:单杀,弱拉,下拉 8:未单杀,弱拉,下拉 9:保持用户赢率为定值	
	
	if @WinRateAsOfLast < @TargetControlRate
		set @PushUp = 1
	if @PushUp = 1 and @InPowerControl = 1 --强上拉
		set @StopPos = 1
	if @PushUp = 0 and @InPowerControl = 1 --强下拉
		set @StopPos = 10
	print '强弱拉停止位置@StopPos:' + isnull(cast(@StopPos as varchar(20)),0)

	select @BeforePrenium = Premium, @BeforeSelectTypeNum = Number, @BeforeSelectTypeColor = Colour from [9lottery].dbo.tab_Games where TypeID = @InTypeID and IssueNumber = @InCurrentIssueNumber
	print '更改前随机数:' + @BeforePrenium + ',更改前中奖数字:' + @BeforeSelectTypeNum + ',更改前中奖颜色:' + @BeforeSelectTypeColor
	
	--去除单杀中的中奖结果
	select top 1 @UserControlType = SelectType from #UserControledBonus order by TotalBonus desc
	if @UserControlType is not null and @UserControlType <> ''
	begin
		print '去除单杀中奖结果类型@UserControlType:' + isnull(cast(@UserControlType as varchar(20)),0)
		--区分颜色和数字
		set @IsUserControl = 1 --单杀使能
		if @UserControlType in ('0','1','2','3','4','5','6','7','8','9')
		begin
			delete from #LotteryResultFinal where SelectTypeNum = @UserControlType
			set @StepCounts = 9
			if @PushUp = 0 and @InPowerControl = 1 --强下拉
				set @StopPos = 9 
		end
		else if @UserControlType in ('red','green','violet', 'big', 'small')
		begin  
			delete from #LotteryResultFinal where charindex(@UserControlType, SelectTypeColor) > 0
			if @UserControlType = '@violet'
			begin
				set @StepCounts = 8
				if @PushUp = 0 and @InPowerControl = 1 --强下拉
					set @StopPos = 8
			end
			else
			begin 
				set @StepCounts = 5
				if @PushUp = 0 and @InPowerControl = 1 --强下拉
					set @StopPos = 5
			end
		end
	end

	declare CursorUpdate cursor for select IssueNumber, SelectTypeNum, SelectTypeColor, WinRate 
			from #LotteryResultFinal where TypeID = @InTypeID ORDER BY WinRate desc
	open CursorUpdate
	fetch next from CursorUpdate into @IssueNumber, @SelectTypeNum, @SelectTypeColor, @WinRate
	while @@FETCH_STATUS = 0
	begin
		set @Loops = @Loops + 1
		if @InPowerControl > 0 --设置了强弱拉回
		begin
			--拉回到设定值(强弱两种方式)@PowerControl->1:强拉 2:弱拉
			if @InPowerControl = 1
			begin 
				if @StopPos = @Loops 
				begin 
					set @LogTypeNum = @SelectTypeNum
					set @LogTypeColor = @SelectTypeColor
					if @IsUserControl = 1 and @PushUp = 1
						set @LogControlType = 1 --单杀,强拉,上拉
					else if @IsUserControl = 1 and @PushUp = 0
						set @LogControlType = 2 --单杀,强拉,下拉
					else if @IsUserControl = 0 and @PushUp = 1
						set @LogControlType = 3 --未单杀,强拉,上拉
					else if @IsUserControl = 0 and @PushUp = 0
						set  @LogControlType = 4 --未单杀,强拉,下拉
					GOTO UpdateAndInsertLog --记录日志
				end
				else
				begin
					fetch next from CursorUpdate into @IssueNumber, @SelectTypeNum, @SelectTypeColor, @WinRate
					continue
				end
			end
			else if @InPowerControl = 2
			begin
				if @PushUp = 1	--上拉找大值
				begin
					if @Loops = 1 --@WinRate都比@WinRateAsOfLast小
					begin
						set @FinalTypeNum = @SelectTypeNum
						set @FinalTypeColor = @SelectTypeColor
					end
					print '弱拉，上拉@WinRate:' + isnull(cast(@WinRate as varchar(20)),0)
					print '弱拉，上拉@WinRateAsOfLast:' + isnull(cast(@WinRateAsOfLast as varchar(20)),0)
					if @WinRate > @WinRateAsOfLast
					begin 
						set @FinalTypeNum = @SelectTypeNum
						set @FinalTypeColor = @SelectTypeColor
						set @BingoCounts = @BingoCounts + 1
					end
					if @BingoCounts = 3 or @Loops = @StepCounts --遍历到第三个大的值或结束
					begin
						print '弱拉，上拉@BingoCounts:' + isnull(cast(@BingoCounts as varchar(20)),0) 
						print '弱拉，上拉@StepCounts:' + isnull(cast(@StepCounts as varchar(20)),0)
						set @LogTypeNum = @FinalTypeNum
						set @LogTypeColor = @FinalTypeColor
						print '弱拉，上拉@LogTypeNum:' + isnull(cast(@LogTypeNum as varchar(20)),0) 
						print '弱拉，上拉@LogTypeColor:' + isnull(cast(@LogTypeColor as varchar(20)),0)
						if @IsUserControl = 1 
							set @LogControlType = 5 --单杀,弱拉,上拉
						else 
							set @LogControlType = 6 --未单杀,弱拉,上拉
						GOTO UpdateAndInsertLog --记录日志
					end
					fetch next from CursorUpdate into @IssueNumber, @SelectTypeNum, @SelectTypeColor, @WinRate
					continue
				end
				else	--下拉找小值
				begin
					print '弱拉，下拉@WinRate:' + isnull(cast(@WinRate as varchar(20)),0)
					print '弱拉，下拉@WinRateAsOfLast:' + isnull(cast(@WinRateAsOfLast as varchar(20)),0)
					if @WinRate < @WinRateAsOfLast and @IsFound = 0  
					begin 
						set @FirstLowWinRatePos = @Loops
						set @IsFound = 1
						print '弱拉，下拉@FirstLowWinRatePos:' + isnull(cast(@FirstLowWinRatePos as varchar(20)),0) 
						if @FirstLowWinRatePos >= @StepCounts - 2
						begin
							print '弱拉，下拉@FirstLowWinRatePos:' + isnull(cast(@FirstLowWinRatePos as varchar(20)),0) 
							print '弱拉，下拉@StepCounts:' + isnull(cast(@StepCounts as varchar(20)),0) 
							set @LogTypeNum = @SelectTypeNum
							set @LogTypeColor = @SelectTypeColor
							print '弱拉，下拉@LogTypeNum:' + isnull(cast(@LogTypeNum as varchar(20)),0) 
							print '弱拉，下拉@LogTypeColor:' + isnull(cast(@LogTypeColor as varchar(20)),0) 
							if @IsUserControl = 1 
								set @LogControlType = 7 --单杀,弱拉,下拉
							else 
								set @LogControlType = 8 --未单杀,弱拉,下拉
							GOTO UpdateAndInsertLog --记录日志
						end
						else
							set @StopPos = @StepCounts -2
					end
					if @Loops = @StopPos or @Loops = @StepCounts 
					begin
						set @LogTypeNum = @SelectTypeNum
						set @LogTypeColor = @SelectTypeColor
						print '1弱拉，下拉@FirstLowWinRatePos:' + isnull(cast(@FirstLowWinRatePos as varchar(20)),0) 
						print '1弱拉，下拉@StopPos:' + isnull(cast(@StopPos as varchar(20)),0) 
						print '1弱拉，下拉@StepCounts:' + isnull(cast(@StepCounts as varchar(20)),0) 
						print '1弱拉，下拉@LogTypeNum:' + isnull(cast(@LogTypeNum as varchar(20)),0) 
						print '1弱拉，下拉@LogTypeColor:' + isnull(cast(@LogTypeColor as varchar(20)),0)
						if @IsUserControl = 1 
							set @LogControlType = 7 --单杀,弱拉,下拉
						else 
							set @LogControlType = 8 --未单杀,弱拉,下拉
						GOTO UpdateAndInsertLog --记录日志
					end
					fetch next from CursorUpdate into @IssueNumber, @SelectTypeNum, @SelectTypeColor, @WinRate
					continue
				end
			end
		end
		else
		begin
			--保持用户赢率在设定值
			if @Loops = @StepCounts
			begin
				print '控制杀率到固定值@StepCounts:' + isnull(cast(@StepCounts as varchar(20)),0)
				set @LogTypeNum = @SelectTypeNum
				set @LogTypeColor = @SelectTypeColor
				set @LogControlType = 9 --保持用户赢率为定值
				GOTO UpdateAndInsertLog --记录日志
			end
			if @WinRate <= @TargetControlRate
			begin
				print '1控制杀率到固定值@WinRate:' + isnull(cast(@WinRate as varchar(20)),0)
				print '1控制杀率到固定值@TargetControlRate:' + isnull(cast(@TargetControlRate as varchar(20)),0)
				set @LogTypeNum = @SelectTypeNum
				set @LogTypeColor = @SelectTypeColor
				set @LogControlType = 9 --保持用户赢率为定值
				GOTO UpdateAndInsertLog --记录日志
			end
			fetch next from CursorUpdate into @IssueNumber, @SelectTypeNum, @SelectTypeColor, @WinRate
			continue
		end
	UpdateAndInsertLog:
		print 'UpdateAndInsertLog' 
		set @RandNumVar = substring(@BeforePrenium, 0, 5) + @LogTypeNum
		update [9lottery].dbo.tab_Games set Premium = @RandNumVar, Number = @LogTypeNum, Colour = @LogTypeColor
			where TypeID = @InTypeID and IssueNumber = @IssueNumber
		insert into [9lottery].dbo.tab_Game_Control_Log(TypeID, IssueNumber, OldPremium, OldNumber, OldColour, NewPremium, NewNumber, NewColour, ControlType, UpdateTime)
			values(@InTypeID, @IssueNumber, @BeforePrenium, @BeforeSelectTypeNum, @BeforeSelectTypeColor, @RandNumVar, @LogTypeNum, @LogTypeColor, @LogControlType, getdate())
		break
	end

	close CursorUpdate
	deallocate CursorUpdate
	
	drop table #LotteryTotalBonus
	drop table #UserControledBonus
	drop table #LotteryResultFinal
END









GO


