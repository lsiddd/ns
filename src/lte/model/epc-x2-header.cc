/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2012 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Author: Manuel Requena <manuel.requena@cttc.es>
 */

#include "ns3/log.h"
#include "ns3/epc-x2-header.h"


NS_LOG_COMPONENT_DEFINE ("EpcX2Header");

namespace ns3 {


NS_OBJECT_ENSURE_REGISTERED (EpcX2Header);

EpcX2Header::EpcX2Header ()
  : m_messageType (0xfa),
    m_procedureCode (0xfa),
    m_lengthOfIes (0xfa),
    m_numberOfIes (0xfa)
{
}

EpcX2Header::~EpcX2Header ()
{
  m_messageType = 0xfb;
  m_procedureCode = 0xfb;
  m_lengthOfIes = 0xfb;
  m_numberOfIes = 0xfb;
}

TypeId
EpcX2Header::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::EpcX2Header")
    .SetParent<Header> ()
    .AddConstructor<EpcX2Header> ()
  ;
  return tid;
}

TypeId
EpcX2Header::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}

uint32_t
EpcX2Header::GetSerializedSize (void) const
{
  return 7;
}

void
EpcX2Header::Serialize (Buffer::Iterator start) const
{
  Buffer::Iterator i = start;

  i.WriteU8 (m_messageType);
  i.WriteU8 (m_procedureCode);

  i.WriteU8 (0x00); // criticality = REJECT
  i.WriteU8 (m_lengthOfIes + 3);
  i.WriteHtonU16 (0);
  i.WriteU8 (m_numberOfIes);
}

uint32_t
EpcX2Header::Deserialize (Buffer::Iterator start)
{
  Buffer::Iterator i = start;

  m_messageType = i.ReadU8 ();
  m_procedureCode = i.ReadU8 ();

  i.ReadU8 ();
  m_lengthOfIes = i.ReadU8 () - 3;
  i.ReadNtohU16 ();
  m_numberOfIes = i.ReadU8 ();
  
  return GetSerializedSize ();
}

void
EpcX2Header::Print (std::ostream &os) const
{
  os << "MessageType=" << (uint32_t) m_messageType;
  os << " ProcedureCode=" << (uint32_t) m_procedureCode;
  os << " LengthOfIEs=" << (uint32_t) m_lengthOfIes;
  os << " NumberOfIEs=" << (uint32_t) m_numberOfIes;
}

uint8_t
EpcX2Header::GetMessageType () const
{
  return m_messageType;
}

void
EpcX2Header::SetMessageType (uint8_t messageType)
{
  m_messageType = messageType;
}

uint8_t
EpcX2Header::GetProcedureCode () const
{
  return m_procedureCode;
}

void
EpcX2Header::SetProcedureCode (uint8_t procedureCode)
{
  m_procedureCode = procedureCode;
}


void
EpcX2Header::SetLengthOfIes (uint32_t lengthOfIes)
{
  m_lengthOfIes = lengthOfIes;
}

void
EpcX2Header::SetNumberOfIes (uint32_t numberOfIes)
{
  m_numberOfIes = numberOfIes;
}

/////////////////////////////////////////////////////////////////////

NS_OBJECT_ENSURE_REGISTERED (EpcX2HandoverRequestHeader);

EpcX2HandoverRequestHeader::EpcX2HandoverRequestHeader ()
  : m_numberOfIes (1 + 1 + 1 + 1),
    m_headerLength (6 + 5 + 12 + (3 + 8 + 8 + 4)),
    m_oldEnbUeX2apId (0xfffa),
    m_cause (0xfffa),
    m_targetCellId (0xfffa)
{
  m_erabsToBeSetupList.clear ();
}

EpcX2HandoverRequestHeader::~EpcX2HandoverRequestHeader ()
{
  m_numberOfIes = 0;
  m_headerLength = 0;
  m_oldEnbUeX2apId = 0xfffb;
  m_cause = 0xfffb;
  m_targetCellId = 0xfffb;
  m_erabsToBeSetupList.clear ();
}

TypeId
EpcX2HandoverRequestHeader::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::EpcX2HandoverRequestHeader")
    .SetParent<Header> ()
    .AddConstructor<EpcX2HandoverRequestHeader> ()
  ;
  return tid;
}

TypeId
EpcX2HandoverRequestHeader::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}

uint32_t
EpcX2HandoverRequestHeader::GetSerializedSize (void) const
{
  return m_headerLength;
}

void
EpcX2HandoverRequestHeader::Serialize (Buffer::Iterator start) const
{
  Buffer::Iterator i = start;

  i.WriteHtonU16 (10);              // id = OLD_ENB_UE_X2AP_ID
  i.WriteU8 (0);                    // criticality = REJECT
  i.WriteU8 (2);                    // length of OLD_ENB_UE_X2AP_ID
  i.WriteHtonU16 (m_oldEnbUeX2apId);

  i.WriteHtonU16 (5);               // id = CAUSE
  i.WriteU8 (1 << 6);               // criticality = IGNORE
  i.WriteU8 (1);                    // length of CAUSE
  i.WriteU8 (m_cause);

  i.WriteHtonU16 (11);              // id = TARGET_CELLID
  i.WriteU8 (0);                    // criticality = REJECT
  i.WriteU8 (8);                    // length of TARGET_CELLID
  i.WriteHtonU32 (0x123456);        // fake PLMN
  i.WriteHtonU32 (m_targetCellId << 4);

  i.WriteHtonU16 (14);              // id = UE_CONTEXT_INFORMATION
  i.WriteU8 (0);                    // criticality = REJECT

  i.WriteHtonU64 (m_ueAggregateMaxBitRateDownlink);
  i.WriteHtonU64 (m_ueAggregateMaxBitRateUplink);

  std::vector <EpcX2Sap::ErabToBeSetupItem>::size_type sz = m_erabsToBeSetupList.size (); 
  i.WriteHtonU32 (sz);              // number of bearers
  for (int j = 0; j < (int) sz; j++)
    {
      i.WriteHtonU16 (m_erabsToBeSetupList [j].erabId);
      i.WriteHtonU16 (m_erabsToBeSetupList [j].erabLevelQosParameters.qci);
      i.WriteHtonU64 (m_erabsToBeSetupList [j].erabLevelQosParameters.gbrQosInfo.gbrDl);
      i.WriteHtonU64 (m_erabsToBeSetupList [j].erabLevelQosParameters.gbrQosInfo.gbrUl);
      i.WriteHtonU64 (m_erabsToBeSetupList [j].erabLevelQosParameters.gbrQosInfo.mbrDl);
      i.WriteHtonU64 (m_erabsToBeSetupList [j].erabLevelQosParameters.gbrQosInfo.mbrUl);
      i.WriteU8 (m_erabsToBeSetupList [j].erabLevelQosParameters.arp.priorityLevel);
      i.WriteU8 (m_erabsToBeSetupList [j].erabLevelQosParameters.arp.preemptionCapability);
      i.WriteU8 (m_erabsToBeSetupList [j].erabLevelQosParameters.arp.preemptionVulnerability);
      i.WriteU8 (m_erabsToBeSetupList [j].dlForwarding);
      i.WriteHtonU32 (m_erabsToBeSetupList [j].transportLayerAddress.Get ());
      i.WriteHtonU32 (m_erabsToBeSetupList [j].gtpTeid);
    }

}

uint32_t
EpcX2HandoverRequestHeader::Deserialize (Buffer::Iterator start)
{
  Buffer::Iterator i = start;

  m_headerLength = 0;
  m_numberOfIes = 0;

  i.ReadNtohU16 ();
  i.ReadU8 ();
  i.ReadU8 ();
  m_oldEnbUeX2apId = i.ReadNtohU16 ();
  m_headerLength += 6;
  m_numberOfIes++;

  i.ReadNtohU16 ();
  i.ReadU8 ();
  i.ReadU8 ();
  m_cause = i.ReadU8 ();
  m_headerLength += 5;
  m_numberOfIes++;
  
  i.ReadNtohU16 ();
  i.ReadU8 ();
  i.ReadU8 ();
  i.ReadNtohU32 ();
  m_targetCellId = i.ReadNtohU32 () >> 4;
  m_headerLength += 12;
  m_numberOfIes++;

  i.ReadNtohU16 ();
  i.ReadU8 ();
  m_ueAggregateMaxBitRateDownlink = i.ReadNtohU64 ();
  m_ueAggregateMaxBitRateUplink   = i.ReadNtohU64 ();
  int sz = i.ReadNtohU32 ();
  m_headerLength += 23;
  m_numberOfIes++;

  for (int j = 0; j < sz; j++)
    {
      EpcX2Sap::ErabToBeSetupItem erabItem;

      erabItem.erabId = i.ReadNtohU16 ();
 
      erabItem.erabLevelQosParameters = EpsBearer ((EpsBearer::Qci) i.ReadNtohU16 ());
      erabItem.erabLevelQosParameters.gbrQosInfo.gbrDl = i.ReadNtohU64 ();
      erabItem.erabLevelQosParameters.gbrQosInfo.gbrUl = i.ReadNtohU64 ();
      erabItem.erabLevelQosParameters.gbrQosInfo.mbrDl = i.ReadNtohU64 ();
      erabItem.erabLevelQosParameters.gbrQosInfo.mbrUl = i.ReadNtohU64 ();
      erabItem.erabLevelQosParameters.arp.priorityLevel = i.ReadU8 ();
      erabItem.erabLevelQosParameters.arp.preemptionCapability = i.ReadU8 ();
      erabItem.erabLevelQosParameters.arp.preemptionVulnerability = i.ReadU8 ();

      erabItem.dlForwarding = i.ReadU8 ();
      erabItem.transportLayerAddress = Ipv4Address (i.ReadNtohU32 ());
      erabItem.gtpTeid = i.ReadNtohU32 ();

      m_erabsToBeSetupList.push_back (erabItem);
      m_headerLength += 48;
    }

  return GetSerializedSize ();
}

void
EpcX2HandoverRequestHeader::Print (std::ostream &os) const
{
  os << "OldEnbUeX2apId=" << m_oldEnbUeX2apId;
  os << " Cause=" << m_cause;
  os << " TargetCellId=" << m_targetCellId;
  os << " UeAggrMaxBitRateDownlink= " << m_ueAggregateMaxBitRateDownlink;
  os << " UeAggrMaxBitRateUplink= " << m_ueAggregateMaxBitRateUplink;
  os << " NumOfBearers=" << m_erabsToBeSetupList.size ();

  std::vector <EpcX2Sap::ErabToBeSetupItem>::size_type sz = m_erabsToBeSetupList.size ();
  if (sz > 0)
    {
      os << " [";
    }
  for (int j = 0; j < (int) sz; j++)
    {
      os << m_erabsToBeSetupList[j].erabId;
      if (j < (int) sz - 1)
        {
          os << ", ";
        }
      else
        {
          os << "]";
        }
    }
}

uint16_t
EpcX2HandoverRequestHeader::GetOldEnbUeX2apId () const
{
  return m_oldEnbUeX2apId;
}

void
EpcX2HandoverRequestHeader::SetOldEnbUeX2apId (uint16_t x2apId)
{
  m_oldEnbUeX2apId = x2apId;
}

uint16_t
EpcX2HandoverRequestHeader::GetCause () const
{
  return m_cause;
}

void
EpcX2HandoverRequestHeader::SetCause (uint16_t cause)
{
  m_cause = cause;
}

uint16_t
EpcX2HandoverRequestHeader::GetTargetCellId () const
{
  return m_targetCellId;
}

void
EpcX2HandoverRequestHeader::SetTargetCellId (uint16_t targetCellId)
{
  m_targetCellId = targetCellId;
}

std::vector <EpcX2Sap::ErabToBeSetupItem>
EpcX2HandoverRequestHeader::GetBearers () const
{
  return m_erabsToBeSetupList;
}

void
EpcX2HandoverRequestHeader::SetBearers (std::vector <EpcX2Sap::ErabToBeSetupItem> bearers)
{
  m_headerLength += 48 * bearers.size ();
  m_erabsToBeSetupList = bearers;
}

uint64_t
EpcX2HandoverRequestHeader::GetUeAggregateMaxBitRateDownlink () const
{
  return m_ueAggregateMaxBitRateDownlink;
}

void
EpcX2HandoverRequestHeader::SetUeAggregateMaxBitRateDownlink (uint64_t bitRate)
{
  m_ueAggregateMaxBitRateDownlink = bitRate;
}

uint64_t
EpcX2HandoverRequestHeader::GetUeAggregateMaxBitRateUplink () const
{
  return m_ueAggregateMaxBitRateUplink;
}

void
EpcX2HandoverRequestHeader::SetUeAggregateMaxBitRateUplink (uint64_t bitRate)
{
  m_ueAggregateMaxBitRateUplink = bitRate;
}

uint32_t
EpcX2HandoverRequestHeader::GetLengthOfIes () const
{
  return m_headerLength;
}

uint32_t
EpcX2HandoverRequestHeader::GetNumberOfIes () const
{
  return m_numberOfIes;
}

/////////////////////////////////////////////////////////////////////

NS_OBJECT_ENSURE_REGISTERED (EpcX2HandoverRequestAckHeader);

EpcX2HandoverRequestAckHeader::EpcX2HandoverRequestAckHeader ()
  : m_numberOfIes (1 + 1 + 1 + 1),
    m_headerLength (2 + 2 + 4 + 4),
    m_oldEnbUeX2apId (0xfffa),
    m_newEnbUeX2apId (0xfffa)
{
}

EpcX2HandoverRequestAckHeader::~EpcX2HandoverRequestAckHeader ()
{
  m_numberOfIes = 0;
  m_headerLength = 0;
  m_oldEnbUeX2apId = 0xfffb;
  m_newEnbUeX2apId = 0xfffb;
  m_erabsAdmittedList.clear ();
  m_erabsNotAdmittedList.clear ();
}

TypeId
EpcX2HandoverRequestAckHeader::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::EpcX2HandoverRequestAckHeader")
    .SetParent<Header> ()
    .AddConstructor<EpcX2HandoverRequestAckHeader> ()
  ;
  return tid;
}

TypeId
EpcX2HandoverRequestAckHeader::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}

uint32_t
EpcX2HandoverRequestAckHeader::GetSerializedSize (void) const
{
  return m_headerLength;
}

void
EpcX2HandoverRequestAckHeader::Serialize (Buffer::Iterator start) const
{
  Buffer::Iterator i = start;

  i.WriteHtonU16 (m_oldEnbUeX2apId);
  i.WriteHtonU16 (m_newEnbUeX2apId);

  std::vector <EpcX2Sap::ErabAdmittedItem>::size_type sz = m_erabsAdmittedList.size (); 
  i.WriteHtonU32 (sz);
  for (int j = 0; j < (int) sz; j++)
    {
      i.WriteHtonU16 (m_erabsAdmittedList [j].erabId);
      i.WriteHtonU32 (m_erabsAdmittedList [j].ulGtpTeid);
      i.WriteHtonU32 (m_erabsAdmittedList [j].dlGtpTeid);
    }

  std::vector <EpcX2Sap::ErabNotAdmittedItem>::size_type sz2 = m_erabsNotAdmittedList.size (); 
  i.WriteHtonU32 (sz2);
  for (int j = 0; j < (int) sz2; j++)
    {
      i.WriteHtonU16 (m_erabsNotAdmittedList [j].erabId);
      i.WriteHtonU16 (m_erabsNotAdmittedList [j].cause);
    }
}

uint32_t
EpcX2HandoverRequestAckHeader::Deserialize (Buffer::Iterator start)
{
  Buffer::Iterator i = start;

  m_headerLength = 0;
  m_numberOfIes = 0;

  m_oldEnbUeX2apId = i.ReadNtohU16 ();
  m_newEnbUeX2apId = i.ReadNtohU16 ();
  m_headerLength += 4;
  m_numberOfIes += 2;

  int sz = i.ReadNtohU32 ();
  m_headerLength += 4;
  m_numberOfIes++;

  for (int j = 0; j < sz; j++)
    {
      EpcX2Sap::ErabAdmittedItem erabItem;

      erabItem.erabId = i.ReadNtohU16 ();
      erabItem.ulGtpTeid = i.ReadNtohU32 ();
      erabItem.dlGtpTeid = i.ReadNtohU32 ();

      m_erabsAdmittedList.push_back (erabItem);
      m_headerLength += 6;
    }

  sz = i.ReadNtohU32 ();
  m_headerLength += 4;
  m_numberOfIes++;

  for (int j = 0; j < sz; j++)
    {
      EpcX2Sap::ErabNotAdmittedItem erabItem;

      erabItem.erabId = i.ReadNtohU16 ();
      erabItem.cause  = i.ReadNtohU16 ();

      m_erabsNotAdmittedList.push_back (erabItem);
      m_headerLength += 4;
    }

  return GetSerializedSize ();
}

void
EpcX2HandoverRequestAckHeader::Print (std::ostream &os) const
{
  os << "OldEnbUeX2apId=" << m_oldEnbUeX2apId;
  os << " NewEnbUeX2apId=" << m_newEnbUeX2apId;

  os << " AdmittedBearers=" << m_erabsAdmittedList.size ();
  std::vector <EpcX2Sap::ErabAdmittedItem>::size_type sz = m_erabsAdmittedList.size ();
  if (sz > 0)
    {
      os << " [";
    }
  for (int j = 0; j < (int) sz; j++)
    {
      os << m_erabsAdmittedList[j].erabId;
      if (j < (int) sz - 1)
        {
          os << ", ";
        }
      else
        {
          os << "]";
        }
    }
  
  os << " NotAdmittedBearers=" << m_erabsNotAdmittedList.size ();
  std::vector <EpcX2Sap::ErabNotAdmittedItem>::size_type sz2 = m_erabsNotAdmittedList.size ();
  if (sz2 > 0)
    {
      os << " [";
    }
  for (int j = 0; j < (int) sz2; j++)
    {
      os << m_erabsNotAdmittedList[j].erabId;
      if (j < (int) sz2 - 1)
        {
          os << ", ";
        }
      else
        {
          os << "]";
        }
    }

}

uint16_t
EpcX2HandoverRequestAckHeader::GetOldEnbUeX2apId () const
{
  return m_oldEnbUeX2apId;
}

void
EpcX2HandoverRequestAckHeader::SetOldEnbUeX2apId (uint16_t x2apId)
{
  m_oldEnbUeX2apId = x2apId;
}

uint16_t
EpcX2HandoverRequestAckHeader::GetNewEnbUeX2apId () const
{
  return m_newEnbUeX2apId;
}

void
EpcX2HandoverRequestAckHeader::SetNewEnbUeX2apId (uint16_t x2apId)
{
  m_newEnbUeX2apId = x2apId;
}

std::vector <EpcX2Sap::ErabAdmittedItem> 
EpcX2HandoverRequestAckHeader::GetAdmittedBearers () const
{
  return m_erabsAdmittedList;
}

void
EpcX2HandoverRequestAckHeader::SetAdmittedBearers (std::vector <EpcX2Sap::ErabAdmittedItem> bearers)
{
  m_headerLength += 10 * bearers.size ();
  m_erabsAdmittedList = bearers;
}

std::vector <EpcX2Sap::ErabNotAdmittedItem>
EpcX2HandoverRequestAckHeader::GetNotAdmittedBearers () const
{
  return m_erabsNotAdmittedList;
}

void
EpcX2HandoverRequestAckHeader::SetNotAdmittedBearers (std::vector <EpcX2Sap::ErabNotAdmittedItem> bearers)
{
  m_headerLength += 4 * bearers.size ();
  m_erabsNotAdmittedList = bearers;
}

uint32_t
EpcX2HandoverRequestAckHeader::GetLengthOfIes () const
{
  return m_headerLength;
}

uint32_t
EpcX2HandoverRequestAckHeader::GetNumberOfIes () const
{
  return m_numberOfIes;
}

/////////////////////////////////////////////////////////////////////

NS_OBJECT_ENSURE_REGISTERED (EpcX2UeContextReleaseHeader);

EpcX2UeContextReleaseHeader::EpcX2UeContextReleaseHeader ()
  : m_numberOfIes (1 + 1),
    m_headerLength (2 + 2),
    m_oldEnbUeX2apId (0xfffa),
    m_newEnbUeX2apId (0xfffa)
{
}

EpcX2UeContextReleaseHeader::~EpcX2UeContextReleaseHeader ()
{
  m_numberOfIes = 0;
  m_headerLength = 0;
  m_oldEnbUeX2apId = 0xfffb;
  m_newEnbUeX2apId = 0xfffb;
}

TypeId
EpcX2UeContextReleaseHeader::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::EpcX2UeContextReleaseHeader")
    .SetParent<Header> ()
    .AddConstructor<EpcX2UeContextReleaseHeader> ()
  ;
  return tid;
}

TypeId
EpcX2UeContextReleaseHeader::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}

uint32_t
EpcX2UeContextReleaseHeader::GetSerializedSize (void) const
{
  return m_headerLength;
}

void
EpcX2UeContextReleaseHeader::Serialize (Buffer::Iterator start) const
{
  Buffer::Iterator i = start;

  i.WriteHtonU16 (m_oldEnbUeX2apId);
  i.WriteHtonU16 (m_newEnbUeX2apId);
}

uint32_t
EpcX2UeContextReleaseHeader::Deserialize (Buffer::Iterator start)
{
  Buffer::Iterator i = start;

  m_oldEnbUeX2apId = i.ReadNtohU16 ();
  m_newEnbUeX2apId = i.ReadNtohU16 ();
  m_numberOfIes = 2;
  m_headerLength = 4;

  return GetSerializedSize ();
}

void
EpcX2UeContextReleaseHeader::Print (std::ostream &os) const
{
  os << "OldEnbUeX2apId=" << m_oldEnbUeX2apId;
  os << " NewEnbUeX2apId=" << m_newEnbUeX2apId;
}

uint16_t
EpcX2UeContextReleaseHeader::GetOldEnbUeX2apId () const
{
  return m_oldEnbUeX2apId;
}

void
EpcX2UeContextReleaseHeader::SetOldEnbUeX2apId (uint16_t x2apId)
{
  m_oldEnbUeX2apId = x2apId;
}

uint16_t
EpcX2UeContextReleaseHeader::GetNewEnbUeX2apId () const
{
  return m_newEnbUeX2apId;
}

void
EpcX2UeContextReleaseHeader::SetNewEnbUeX2apId (uint16_t x2apId)
{
  m_newEnbUeX2apId = x2apId;
}

uint32_t
EpcX2UeContextReleaseHeader::GetLengthOfIes () const
{
  return m_headerLength;
}

uint32_t
EpcX2UeContextReleaseHeader::GetNumberOfIes () const
{
  return m_numberOfIes;
}

/////////////////////////////////

NS_OBJECT_ENSURE_REGISTERED (EpcX2LoadInformationHeader);

EpcX2LoadInformationHeader::EpcX2LoadInformationHeader ()
  : m_numberOfIes (1),
    m_headerLength (6)
{
  m_cellInformationList.clear ();
}

EpcX2LoadInformationHeader::~EpcX2LoadInformationHeader ()
{
  m_numberOfIes = 0;
  m_headerLength = 0;
  m_cellInformationList.clear ();
}

TypeId
EpcX2LoadInformationHeader::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::EpcX2LoadInformationHeader")
    .SetParent<Header> ()
    .AddConstructor<EpcX2LoadInformationHeader> ()
  ;
  return tid;
}

TypeId
EpcX2LoadInformationHeader::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}

uint32_t
EpcX2LoadInformationHeader::GetSerializedSize (void) const
{
  return m_headerLength;
}

void
EpcX2LoadInformationHeader::Serialize (Buffer::Iterator start) const
{
  Buffer::Iterator i = start;

  i.WriteHtonU16 (6);               // id = CELL_INFORMATION
  i.WriteU8 (1 << 6);               // criticality = IGNORE
  i.WriteU8 (4);                    // length of CELL_INFORMATION_ID

  std::vector <EpcX2Sap::CellInformationItem>::size_type sz = m_cellInformationList.size ();
  i.WriteHtonU16 (sz);              // number of cellInformationItems

  for (int j = 0; j < (int) sz; j++)
    {
      i.WriteHtonU16 (m_cellInformationList [j].sourceCellId);

      std::vector <EpcX2Sap::UlInterferenceOverloadIndicationItem>::size_type sz2;
      sz2 = m_cellInformationList [j].ulInterferenceOverloadIndicationList.size ();
      i.WriteHtonU16 (sz2);         // number of UlInterferenceOverloadIndicationItem

      for (int k = 0; k < (int) sz2; k++)
        {
          i.WriteU8 (m_cellInformationList [j].ulInterferenceOverloadIndicationList [k]);
        }

      std::vector <EpcX2Sap::UlHighInterferenceInformationItem>::size_type sz3;
      sz3 = m_cellInformationList [j].ulHighInterferenceInformationList.size ();
      i.WriteHtonU16 (sz3);         // number of UlHighInterferenceInformationItem

      for (int k = 0; k < (int) sz3; k++)
        {
          i.WriteHtonU16 (m_cellInformationList [j].ulHighInterferenceInformationList [k].targetCellId);

          std::vector <bool>::size_type sz4;
          sz4 = m_cellInformationList [j].ulHighInterferenceInformationList [k].ulHighInterferenceIndicationList.size ();
          i.WriteHtonU16 (sz4);

          for (int m = 0; m < (int) sz4; m++)
            {
              i.WriteU8 (m_cellInformationList [j].ulHighInterferenceInformationList [k].ulHighInterferenceIndicationList [m]);
            }
        }

      std::vector <bool>::size_type sz5;
      sz5 = m_cellInformationList [j].relativeNarrowbandTxBand.rntpPerPrbList.size ();
      i.WriteHtonU16 (sz5);

      for (int k = 0; k < (int) sz5; k++)
        {
          i.WriteU8 (m_cellInformationList [j].relativeNarrowbandTxBand.rntpPerPrbList [k]);
        }

      i.WriteHtonU16 (m_cellInformationList [j].relativeNarrowbandTxBand.rntpThreshold);
      i.WriteHtonU16 (m_cellInformationList [j].relativeNarrowbandTxBand.antennaPorts);
      i.WriteHtonU16 (m_cellInformationList [j].relativeNarrowbandTxBand.pB);
      i.WriteHtonU16 (m_cellInformationList [j].relativeNarrowbandTxBand.pdcchInterferenceImpact);
    }
}

uint32_t
EpcX2LoadInformationHeader::Deserialize (Buffer::Iterator start)
{
  Buffer::Iterator i = start;

  m_headerLength = 0;
  m_numberOfIes = 0;

  i.ReadNtohU16 ();
  i.ReadU8 ();
  i.ReadU8 ();
  int sz = i.ReadNtohU16 ();
  m_headerLength += 6;
  m_numberOfIes++;

  for (int j = 0; j < sz; j++)
    {
      EpcX2Sap::CellInformationItem cellInfoItem;
      cellInfoItem.sourceCellId = i.ReadNtohU16 ();
      m_headerLength += 2;

      int sz2 = i.ReadNtohU16 ();
      m_headerLength += 2;
      for (int k = 0; k < sz2; k++)
        {
          EpcX2Sap::UlInterferenceOverloadIndicationItem item = (EpcX2Sap::UlInterferenceOverloadIndicationItem) i.ReadU8 ();
          cellInfoItem.ulInterferenceOverloadIndicationList.push_back (item);
        }
      m_headerLength += sz2;

      int sz3 = i.ReadNtohU16 ();
      m_headerLength += 2;
      for (int k = 0; k < sz3; k++)
        {
          EpcX2Sap::UlHighInterferenceInformationItem item;
          item.targetCellId = i.ReadNtohU16 ();
          m_headerLength += 2;

          int sz4 = i.ReadNtohU16 ();
          m_headerLength += 2;
          for (int m = 0; m < sz4; m++)
            {
              item.ulHighInterferenceIndicationList.push_back (i.ReadU8 ());
            }
          m_headerLength += sz4;

          cellInfoItem.ulHighInterferenceInformationList.push_back (item);
        }

      int sz5 = i.ReadNtohU16 ();
      m_headerLength += 2;
      for (int k = 0; k < sz5; k++)
        {
          cellInfoItem.relativeNarrowbandTxBand.rntpPerPrbList.push_back (i.ReadU8 ());
        }
      m_headerLength += sz5;

      cellInfoItem.relativeNarrowbandTxBand.rntpThreshold = i.ReadNtohU16 ();
      cellInfoItem.relativeNarrowbandTxBand.antennaPorts = i.ReadNtohU16 ();
      cellInfoItem.relativeNarrowbandTxBand.pB = i.ReadNtohU16 ();
      cellInfoItem.relativeNarrowbandTxBand.pdcchInterferenceImpact = i.ReadNtohU16 ();
      m_headerLength += 8;

      m_cellInformationList.push_back (cellInfoItem);
    }

  return GetSerializedSize ();
}

void
EpcX2LoadInformationHeader::Print (std::ostream &os) const
{
  os << "NumOfCellInformationItems=" << m_cellInformationList.size ();
}

std::vector <EpcX2Sap::CellInformationItem>
EpcX2LoadInformationHeader::GetCellInformationList () const
{
  return m_cellInformationList;
}

void
EpcX2LoadInformationHeader::SetCellInformationList (std::vector <EpcX2Sap::CellInformationItem> cellInformationList)
{
  m_cellInformationList = cellInformationList;
  m_headerLength += 2;

  std::vector <EpcX2Sap::CellInformationItem>::size_type sz = m_cellInformationList.size ();
  for (int j = 0; j < (int) sz; j++)
    {
      m_headerLength += 2;

      std::vector <EpcX2Sap::UlInterferenceOverloadIndicationItem>::size_type sz2;
      sz2 = m_cellInformationList [j].ulInterferenceOverloadIndicationList.size ();
      m_headerLength += 2 + sz2;

      std::vector <EpcX2Sap::UlHighInterferenceInformationItem>::size_type sz3;
      sz3 = m_cellInformationList [j].ulHighInterferenceInformationList.size ();
      m_headerLength += 2;

      for (int k = 0; k < (int) sz3; k++)
        {
          std::vector <bool>::size_type sz4;
          sz4 = m_cellInformationList [j].ulHighInterferenceInformationList [k].ulHighInterferenceIndicationList.size ();
          m_headerLength += 2 + 2 + sz4;
        }

      std::vector <bool>::size_type sz5;
      sz5 = m_cellInformationList [j].relativeNarrowbandTxBand.rntpPerPrbList.size ();
      m_headerLength += 2 + sz5 + 8;
    }
}

uint32_t
EpcX2LoadInformationHeader::GetLengthOfIes () const
{
  return m_headerLength;
}

uint32_t
EpcX2LoadInformationHeader::GetNumberOfIes () const
{
  return m_numberOfIes;
}


} // namespace ns3