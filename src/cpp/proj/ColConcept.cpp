/**
 * Given a table_id and column number,
 * output top-k concepts describing this column.
 * This is the Naive Majority method.
 */

vector<int> Bridge::findColConceptMajority(int tid, int c, bool print)
{
	Table curTable = corpus->getTableByDataId(tid);
	int totalConcept = kb->countConcept();
	double majorityThreshold = 0.6;

	//check range
	if (c < 0 || c >= curTable.nCol)
	{
		cout << "Column index is out of range!" << endl;
		return vector<int>();
	}

	//Compute total lucky cell in this column
	int numLuckyCell = 0;
	for (int i = 0; i < curTable.nRow; i ++)
		if (! getMatch(curTable.cells[i][c].id).empty())
			numLuckyCell ++;

	//Loop over all concepts
	vector<pair<int, int>> ans;
	for (int i = 1; i <= totalConcept; i ++)
	{
		int numContainedCell = 0;
		for (int j = 0; j < curTable.nRow; j ++)
		{
			vector<int>& curMatches = getMatch(curTable.cells[j][c].id);
			if (curMatches.empty())
				continue;
			for (int entityId : curMatches)
				if (kb->checkRecursiveBelong(entityId, i))
				{
					numContainedCell ++;
					break;
				}
		}
		if ((double) numContainedCell / numLuckyCell >= majorityThreshold)
			ans.emplace_back(- kb->getDepth(i), i);
	}
	//Sort by depth
	sort(ans.begin(), ans.end());
	
	//print
	if (print)
		for (int i = 0; i < ans.size(); i ++)
			cout << kb->getConcept(ans[i].second) << " " << - ans[i].first << endl;

	return ans;
}
