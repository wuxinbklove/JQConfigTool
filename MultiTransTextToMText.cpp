#include "StdAfx.h"
#include "arxHeaders.h"
#include "MultiTransTextToMText.h"
#include "sdk/GoldTools/GLockDocument.h"


MultiTransTextToMText* MultiTransTextToMText::mInstance = NULL;

MultiTransTextToMText* MultiTransTextToMText::GetInstance()
{
	if (mInstance == NULL)
	{
		mInstance = new MultiTransTextToMText();
	}
	mInstance->readConfig();
	return mInstance;

}

MultiTransTextToMText::MultiTransTextToMText()
{
}


MultiTransTextToMText::~MultiTransTextToMText()
{
}

void MultiTransTextToMText::readConfig()
{
	CString xOffsetStr, yOffsetStr;
	::GetPrivateProfileString(_T("MultiTransTextToMText"), _T("XOffset"), _T("0.2"), xOffsetStr.GetBuffer(100), 100, ConfigFile);
	::GetPrivateProfileString(_T("MultiTransTextToMText"), _T("YOffset"), _T("0.0"), yOffsetStr.GetBuffer(100), 100, ConfigFile);
	xOffsetStr.ReleaseBuffer();
	yOffsetStr.ReleaseBuffer();
	m_xOffset = _ttof(xOffsetStr);
	m_yOffset = _ttof(yOffsetStr);

	m_isSetSingleLine = ::GetPrivateProfileInt(_T("MultiTransTextToMText"), _T("IsSetSingleLine"), 1, ConfigFile);

}

void MultiTransTextToMText::start()
{
	GLockDocument lockDoc;

	AcDbBlockTable *pTable;
	Acad::ErrorStatus es;
	es = acdbHostApplicationServices()->workingDatabase()->getBlockTable(pTable, AcDb::kForWrite);
	if (!es)
	{
		Acad::ErrorStatus es2;
		AcDbBlockTableIterator *pTableIter;
		AcDbBlockTableRecord *pTableRcd;
		pTable->newIterator(pTableIter);
		for (pTableIter->start(); !pTableIter->done(); pTableIter->step())
		{
			es2 = pTableIter->getRecord(pTableRcd, AcDb::kForWrite);
			if (!es2)
			{
				Acad::ErrorStatus es3;
				AcDbEntity *pEnt;
				AcDbBlockTableRecordIterator *pTableRcdIter;
				pTableRcd->newIterator(pTableRcdIter);
				for (pTableRcdIter->start(); !pTableRcdIter->done(); pTableRcdIter->step())
				{
					es3 = pTableRcdIter->getEntity(pEnt, AcDb::kForWrite);
					if (!es3)
					{
						if (pEnt->isKindOf(AcDbText::desc()) == Adesk::kTrue
							&& pEnt->isKindOf(AcDbAttributeDefinition::desc()) != Adesk::kTrue
							&& pEnt->isKindOf(AcDbAttribute::desc()) != Adesk::kTrue)
						{
							AcDbText *pText = AcDbText::cast(pEnt);
							AcDbMText *pMtext = new AcDbMText();
							pMtext->setTextStyle(pText->textStyle());
							pMtext->setTextHeight(pText->height());
							double widthFactor = pText->widthFactor();
							CString mTextContent;
							mTextContent.Format(_T("{\\W%lf;%s}"), widthFactor, pText->textString());
							pMtext->setContents(mTextContent);
							pMtext->setRotation(pText->rotation());
							AcDbExtents textExtents;
							pText->getGeomExtents(textExtents);
							AcGePoint3d markPos;
							markPos = pText->position();
							//markPos = textExtents.minPoint();
							markPos.x += m_xOffset;
							markPos.y += m_yOffset;
							pMtext->setLocation(markPos);
							pMtext->setColor(pText->color());
							pMtext->setAttachment(AcDbMText::kBottomLeft);
							pMtext->setLayer(pText->layer());
							pMtext->setLinetype(pText->linetype());
							if (!m_isSetSingleLine)
								pMtext->setWidth(textExtents.maxPoint().x - textExtents.minPoint().x + 10);
							pTableRcd->appendAcDbEntity(pMtext);
							pMtext->close();
							pText->erase();
							pText->close();
						}
						else
						{
							pEnt->close();
						}
					}
				}
				delete pTableRcdIter;
				pTableRcd->close();
			}
		}
		delete pTableIter;
		pTable->close();
		acedCommand(RTSTR, _T("REGEN"), RTNONE);
	}
}